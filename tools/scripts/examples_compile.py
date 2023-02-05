#!/usr/bin/env python3
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os
import sys
import re
import argparse
import platform
import subprocess
import multiprocessing as mp
from pathlib import Path

is_running_in_ci = (os.getenv("CIRCLECI") is not None or
                    os.getenv("TRAVIS") is not None or
                    os.getenv("GITHUB_ACTIONS") is not None)
is_running_on_windows = "Windows" in platform.platform()
build_dir = (Path(os.path.abspath(__file__)).parents[2] / "build")
cache_dir = build_dir / "cache"
global_options = {}
if is_running_in_ci:
    global_options["::build.path"] = "build/"
    global_options[":::cache_dir"] = str(cache_dir)

def run_command(where, command, all_output=False):
    result = subprocess.run(command, shell=True, cwd=where, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output = ""
    if result.returncode or all_output:
        output += result.stdout.decode("utf-8").strip(" \n")
    output += result.stderr.decode("utf-8").strip(" \n")
    return (result.returncode, output)

def generate(project):
    path = project.parent
    output = ["=" * 90, "Generating: {}".format(path)]
    options = " ".join("-D{}={}".format(k, v) for k,v in global_options.items())
    # Compile Linux examples under macOS with hosted-darwin target
    if "hosted-linux" in project.read_text():
        options += " -D:target=hosted-{}".format(platform.system().lower())
    rc, ro = run_command(path, "lbuild {} build".format(options))
    print("\n".join(output + [ro]))
    return None if rc else project

def build(project):
    path = project.parent
    project_cfg = project.read_text()
    commands = []
    if ":build:scons" in project_cfg:
        commands.append( ("scons build --cache-show --random", "SCons") )
    if ":build:make" in project_cfg and not is_running_on_windows:
        commands.append( ("make build", "Make") )
    elif ":build:cmake" in project_cfg:
        build_dir = re.search(r'name=".+?:build:build.path">(.*?)</option>', project_cfg)[1]
        cmd = "cmake -E make_directory {}/cmake-build-release; ".format(build_dir)
        cmd += '(cd {}/cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" {}); '.format(build_dir, path.absolute())
        cmd += "cmake --build {}/cmake-build-release".format(build_dir)
        commands.append( (cmd, "CMake") )

    rcs = 0
    for command, build_system in commands:
        output = ["=" * 90, "Building: {} with {}".format(path / "main.cpp", build_system)]
        rc, ro = run_command(path, command)
        rcs += rc
        print("\n".join(output + [ro]))

    return None if rcs else project

def run(project):
    path = project.parent
    project_cfg = project.read_text()
    commands = []
    if ":build:scons" in project_cfg:
        commands.append( ("scons run", "SCons") )
    if ":build:make" in project_cfg and not is_running_on_windows:
        commands.append( ("make run", "Make") )

    rcs = 0
    for command, build_system in commands:
        output = ["=" * 90, "Running: {} with {}".format(path / "main.cpp", build_system)]
        rc, ro = run_command(path, command, all_output=True)
        print("\n".join(output + [ro]))
        if "CI: run fail" in project_cfg:
            rcs += 0 if rc else 1
        else:
            rcs += rc

    return None if rcs else project

def compile_examples(paths, jobs, split, part):
    print("Using {}x parallelism".format(jobs))
    # Create build folder to prevent process race
    cache_dir.mkdir(exist_ok=True, parents=True)
    (cache_dir / "config").write_text('{"prefix_len": 2}')
    # Validate that paths exist!
    invalid_paths = [p for p in paths if not Path(p).exists()]
    if invalid_paths: print("Invalid paths:\n- " + "\n- ".join(invalid_paths));
    results = len(invalid_paths)
    # Find all project files
    projects = [p for path in paths for p in Path(path).glob("**/project.xml")]
    projects.sort()
    # Split projects up into parts
    if split > 1:
        chunk_size = math.ceil(len(projects) / args.split)
        projects = projects[chunk_size*args.part:min(chunk_size*(args.part+1), len(projects))]

    ctx = mp.get_context("spawn")
    # first generate all projects
    with ctx.Pool(jobs) as pool:
        projects = pool.map(generate, projects)
    results += projects.count(None)

    # Filter projects for successful generation
    projects = [p for p in projects if p is not None]
    # Then build the successfully generated ones
    with ctx.Pool(jobs) as pool:
        projects = pool.map(build, projects)
    results += projects.count(None)

    # Filter projects for successful compilation and runablity
    projects = [p for p in projects if p is not None and "CI: run" in p.read_text()]
    # Then run the successfully compiled ones
    with ctx.Pool(jobs) as pool:
        projects = pool.map(run, projects)
    results += projects.count(None)

    return results


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run platform tests')
    parser.add_argument(
            "paths",
            nargs="+",
            help="Path to search examples in")
    parser.add_argument(
            "--jobs",
            dest="jobs",
            default=os.cpu_count(),
            type=int,
            help="Number of parallel jobs")
    parser.add_argument(
            "--part",
            dest="part",
            default=0,
            type=int,
            help="Execute this part of the splitting.")
    parser.add_argument(
            "--split",
            dest="split",
            default=1,
            type=int,
            help="Split the examples into this many parts.")
    args = parser.parse_args()
    exit(compile_examples(args.paths, args.jobs, args.split, args.part))

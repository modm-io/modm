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
import shutil
import argparse
import platform
import subprocess
from multiprocessing.pool import ThreadPool
from pathlib import Path

is_running_in_ci = (os.getenv("CIRCLECI") is not None or
                    os.getenv("TRAVIS") is not None or
                    os.getenv("GITHUB_ACTIONS") is not None)
is_running_on_windows = "Windows" in platform.platform()
is_running_on_arm64 = "arm64" in platform.machine()
repo_dir = Path(os.path.abspath(__file__)).parents[2]
build_dir = repo_dir / "build"
cache_dir = build_dir / "cache"
repo_file = repo_dir / "repo.lb"
option_collector_pattern = r'<!--(.+?)-->\n\s+<!-- *<(option|collect) +name="(.+?)">(.+?)</(?:option|collect)> *-->'
option_map = {"option": "-D", "collect": "--collect"}
module_pattern = r'<!--(.+?)-->\n\s+<!-- *<module>(.+?)</module> *-->'
global_options = f" -D modm:build:build.path=build/ -D modm:build:scons:cache_dir={cache_dir}" if is_running_in_ci else ""


def run_command(where, command, all_output=False):
    result = subprocess.run(command, shell=True, cwd=where, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output = ""
    if result.returncode or all_output:
        output += result.stdout.decode("utf-8", errors="ignore").strip(" \n")
    output += result.stderr.decode("utf-8", errors="ignore").strip(" \n")
    return (result.returncode, output)

def enable(projects):
    filtered_projects = []
    for project in projects:
        if (query := re.search(r"<!-- CI: enable (.*?) -->", project.read_text())) is not None and not eval(query[1]):
            print(f"Filtering out {project}: {query[1]}")
            continue
        filtered_projects.append(project)
    return filtered_projects

def prepare(project):
    project_cfg = project.read_text()
    configs = set(re.findall(r"<extends>(.+?)</extends>", project_cfg))

    if len(configs) >= 2:
        output = ["=" * 90, f"Preparing: {project.parent}\n"]
        config_options_collectors = re.findall(option_collector_pattern, project_cfg, flags=re.MULTILINE)
        config_modules = re.findall(module_pattern, project_cfg, flags=re.MULTILINE)
        generators = []
        for config in sorted(configs):
            config_name = re.sub(r"[:-]+", "_", config)
            new_project_xml = re.search(r'<option +name="modm:build:build\.path">(.*?)</option>', project_cfg)[1]
            new_project_xml = (project.parent / new_project_xml / config_name.replace("modm_", "") / "project.xml")
            shutil.copytree(project.parent, new_project_xml.parent, dirs_exist_ok=True)
            new_project_cfg = re.sub(r"<extends>.*?</extends>", "", project_cfg)
            new_project_cfg = re.sub(r"<library>", f"<library>\n  <extends>{config}</extends>", new_project_cfg)
            new_project_xml.write_text(new_project_cfg)
            options = "".join(f" {option_map[t]} {k}={v}" for d,t,k,v in config_options_collectors if config in d)
            build_options = "".join(f" -m {m}" for d,m in config_modules if config in d)
            lbuild_options = f"-r {repo_file} -D modm:build:build.path=build/" + options
            generators.append((project, config, lbuild_options, build_options, new_project_xml))
            output.append(f"- {config:30}{options}{build_options}")
        print("\n".join(output))
        return generators

    if '<option name="modm:target">hosted-linux</option>' in project_cfg:
        target = "hosted-" + platform.system().lower()
        if is_running_on_arm64: target += "-arm64"
        return [(project, target, "-D modm:target=" + target, "", project)]

    return [(project, "project.xml", "", "", project)]

def generate(project):
    project, config, lbuild_options, build_options, project_xml = project
    output = ["=" * 90, f"Generating: {project.parent} for {config}"]
    cmd = f"lbuild {global_options} {lbuild_options} build {build_options} --no-log"
    rc, ro = run_command(project_xml.parent, cmd)
    print("\n".join(output + [ro]))
    return None if rc else project_xml.resolve()

def build(project):
    path = project.parent
    project_cfg = project.read_text()
    commands = []
    if ":build:scons" in project_cfg:
        commands.append( ("scons build --cache-show --random", "SCons") )
    if ":build:make" in project_cfg and not is_running_on_windows:
        commands.append( ("make build", "Make") )
    elif ":build:cmake" in project_cfg and not is_running_on_windows:
        build_dir = re.search(r'name=".+?:build:build.path">(.*?)</option>', project_cfg)[1]
        cmd = f"cmake -E make_directory {build_dir}/cmake-build-release; "
        cmd += f'(cd {build_dir}/cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" {path.absolute()}); '
        cmd += f"cmake --build {build_dir}/cmake-build-release"
        commands.append( (cmd, "CMake") )

    rcs = 0
    for command, build_system in commands:
        output = ["=" * 90, f"Building: {path.relative_to(repo_dir)}/main.cpp with {build_system}"]
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
        output = ["=" * 90, f"Running: {path.relative_to(repo_dir)}/main.cpp with {build_system}"]
        rc, ro = run_command(path, command, all_output=True)
        print("\n".join(output + [ro]))
        if "CI: run fail" in project_cfg:
            rcs += 0 if rc else 1
        else:
            rcs += rc

    return None if rcs else project

def compile_examples(paths, jobs, split, part):
    print(f"Using {jobs}x parallelism")
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
    # Filter projects
    projects = enable(projects)

    # first prepare all projects
    with ThreadPool(jobs) as pool:
        projects = pool.map(prepare, projects)
    # Unlistify the project preparations
    projects = [p for plist in projects for p in plist]
    results += projects.count(None)

    # first generate all projects
    with ThreadPool(jobs) as pool:
        projects = pool.map(generate, projects)
    # Unlistify the project configs
    results += projects.count(None)

    # Filter projects for successful generation
    projects = [p for p in projects if p is not None]
    # Then build the successfully generated ones
    with ThreadPool(jobs) as pool:
        projects = pool.map(build, projects)
    results += projects.count(None)

    # Filter projects for successful compilation and runablity
    projects = [p for p in projects if p is not None and "CI: run" in p.read_text()]
    # Then run the successfully compiled ones
    with ThreadPool(jobs) as pool:
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

#!/usr/bin/env python3
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os
import hashlib
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
comment_pattern = re.compile(r"<!--(.*?)-->", flags=re.S)
scoped_pattern = re.compile(r"(<!--(?:(?!-->).)*-->)((?:\s*<(?:option|collect|module)\b[^\n]*)+)", flags=re.S)
element_pattern = re.compile(r'\s*<(option|collect|module)(?: +name="(.+?)")?>(.+?)</\1>\s*')
ignore_patterns = shutil.ignore_patterns("modm", "build", "project.xml.log", "SConstruct", "Makefile", "CMakeLists.txt")
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

def find_board_targets():
    """Returns {config: target} of all board configurations."""
    targets = {}
    for module in (repo_dir / "src/modm/board").glob("*/module.lb"):
        name = "modm:" + re.search(r'\.name += +".*?:board:(.*?)"', module.read_text())[1]
        targets[name] = re.search(r'"modm:target">(.+?)<', (module.parent / "board.xml").read_text())[1]
    # The modm:hosted configuration selects the target of this computer
    targets["modm:hosted"] = "hosted"
    return targets

board_targets = find_board_targets()

def configurations(project_cfg):
    """
    Every `<extends>` is one configuration, also the commented ones.
    A comment naming configurations applies the commented options, collectors
    and modules directly below it to these configurations:

        <!-- Required for modm:blue-pill-f103, modm:hosted -->
        <!-- <option name="modm:build:openocd.cfg">interface/stlink.cfg</option> -->

    Lines below such a comment that are not commented out are the settings of
    the default configuration and are removed for all other configurations:

        <!-- Required for modm:disco-f407vg -->
        <module>modm:platform:can:1</module>

    Returns {config: (lbuild options, lbuild build options)}.
    """
    configs = {c: ([], []) for c in sorted(set(re.findall(r"<extends>(.+?)</extends>", project_cfg)))}
    scope, end = [], 0
    for comment in comment_pattern.finditer(project_cfg):
        adjacent = not project_cfg[end:comment.start()].strip()
        end = comment.end()
        if "<extends>" in comment[1]:
            scope = []
        elif element := element_pattern.fullmatch(comment[1]):
            if not adjacent: scope = []
            kind, name, value = element.groups()
            for config in scope:
                if kind == "option": configs[config][0].append(f"-D {name}={value}")
                elif kind == "collect": configs[config][0].append(f"--collect {name}={value}")
                else: configs[config][1].append(f"-m {value}")
        else:
            # Match whole names only: modm:nucleo-h743zi is not modm:nucleo-h743zi2
            scope = set(re.findall(r"[\w:-]+", comment[1])) & configs.keys()
    return configs

def remove_foreign_settings(project_cfg, config, configs):
    """Removes the active settings that are required for other configurations only."""
    def remove(match):
        scope = set(re.findall(r"[\w:-]+", match[1])) & configs.keys()
        return match[0] if not scope or config in scope else match[1]
    return scoped_pattern.sub(remove, project_cfg)

def targets(project):
    """Returns the targets of all configurations of this project."""
    # The board or target may be inherited from a lbuild.xml in a parent folder
    project_cfg = project.read_text() + "".join(
            f.read_text() for p in project.relative_to(repo_dir).parents[:-1]
            if (f := repo_dir / p / "lbuild.xml").exists())
    if boards := re.findall(r"<extends>(modm:[\w-]+)", project_cfg):
        return [board_targets[b] for b in boards]
    return [re.search(r'"modm:target">(.+?)<', project_cfg)[1]]

def prepare(project):
    build_path = build_dir / project.parent.relative_to(repo_dir)
    project_cfg = project.read_text()
    configs = configurations(project_cfg)

    if len(configs) >= 2:
        output = ["=" * 90, f"Preparing: {project.parent}\n"]
        # Only the example folder is copied, so relative includes of files
        # outside of it are resolved via the original folder.
        generators = []
        base_cfg = re.sub(r" *(<!-- *)?<extends>.*?</extends>( *-->)?\n", "", project_cfg)
        for config, (options, build_options) in configs.items():
            config_name = re.sub(r"[:-]+", "_", config.removeprefix("modm:"))
            new_project_xml = build_path / config_name / "project.xml"
            shutil.copytree(project.parent, new_project_xml.parent, dirs_exist_ok=True, ignore=ignore_patterns)
            target = board_targets[":".join(config.split(":")[:2])]
            new_project_cfg = remove_foreign_settings(base_cfg, config, configs)
            new_project_cfg = new_project_cfg.replace("<library>", f"<library>\n  <extends>{config}</extends>", 1)
            # Only hosted targets can be executed in the CI
            if target != "hosted": new_project_cfg = new_project_cfg.replace("CI: run", "")
            new_project_xml.write_text(new_project_cfg)
            options, build_options = " ".join(options), " ".join(build_options)
            lbuild_options = (f"-r {repo_file} -D modm:build:build.path=build/ "
                              f"--collect modm:build:path.include={project.parent} {options}")
            generators.append((project, config, lbuild_options, build_options, new_project_xml, target))
            output.append(f"- {config:30} {options} {build_options}")
        print("\n".join(output))
        return generators

    return [(project, "project.xml", "", "", project, targets(project)[0])]

def generate(project):
    project, config, lbuild_options, build_options, project_xml, _ = project
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
        if ":build:compilation_db" in project_cfg:
            commands.append( ("scons compilation_db", "CompilationDB (via SCons)") )
    if ":build:make" in project_cfg and not is_running_on_windows:
        commands.append( ("make build", "Make") )
    elif ":build:cmake" in project_cfg and not is_running_on_windows:
        # Inside the build path, which the other build systems do not search for sources
        build_dir = "build" if is_running_in_ci else f"build/{path.name}"
        cmd = f"cmake -E make_directory {build_dir}/cmake-build-release; "
        cmd += f'(cd {build_dir}/cmake-build-release && cmake -DCMAKE_BUILD_TYPE=MinSizeRel -G "Unix Makefiles" {path.absolute()}); '
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

def compile_examples(paths, jobs, split, part, target):
    print(f"Using {jobs}x parallelism")
    # Create build folder to prevent process race
    cache_dir.mkdir(exist_ok=True, parents=True)
    (cache_dir / "config").write_text('{"prefix_len": 2}')
    # Validate that paths exist!
    invalid_paths = [p for p in paths if not Path(p).exists()]
    if invalid_paths: print("Invalid paths:\n- " + "\n- ".join(invalid_paths));
    results = len(invalid_paths)
    # Find all project files
    projects = [p for path in paths for p in Path(path).resolve().glob("**/project.xml")]
    projects.sort()
    # Filter projects
    projects = enable(projects)

    # first prepare all projects
    with ThreadPool(jobs) as pool:
        projects = pool.map(prepare, projects)
    # Unlistify the project preparations
    projects = [p for plist in projects for p in plist if re.search(target, p[-1])]
    # Split configurations up into parts
    # Stable pseudo-random order, so that all parts get a similar mix of examples and targets
    if split > 1:
        projects.sort(key=lambda p: hashlib.md5(f"{os.path.relpath(p[0], repo_dir)} {p[1]}".encode()).digest())
        projects = projects[part::split]

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
    parser.add_argument(
            "--target",
            dest="target",
            default="",
            help="Only compile for targets matching this regex, e.g. '^at' for AVRs.")
    args = parser.parse_args()
    exit(compile_examples(args.paths, args.jobs, args.split, args.part, args.target))

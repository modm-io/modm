#!/usr/bin/env python3
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os
import re
import sys
import time
import shutil
import signal
import subprocess
from pathlib import Path

# =============================================================================
boards = [b.name for b in (Path(__file__).parents[2] / "src/modm/board").iterdir() if b.is_dir()]
boards = sorted([b for b in boards if not b.startswith("_")])
boards_scons_only = ["al_avreb_can", "arduino_uno"]
build_command = {
    "scons": "scons build",
    "cmake": "make cmake && make"
}
cleanup_files = [
    "modm",
    "SConstruct",
    "CMakeLists.txt",
    "Makefile",
    "project_ci.xml",
]

# =============================================================================
def cleanup(where):
    for f in cleanup_files:
        if (where / f).is_dir():
            shutil.rmtree(where / "modm", ignore_errors=True)
        elif os.path.exists(where / f):
            os.remove(where / f)

def run_ci(config, function):
    exec(function, globals())
    for r in ci_next(config, boards):
        builds = list(build_command) if r[1] == "*" else r[1]
        if (r[2] in boards_scons_only) and "cmake" in builds:
            builds.remove("cmake")
        yield (r[0], builds, r[2])

def config_parser(project, config):
    match = re.search(r"( *?<!--).*?\n( *?)\.\.ci:.*?\n( *?-->)\n", config, flags=re.DOTALL)
    if match:
        config = config.replace(match.group(0), "")
        function = match.group(0)\
                   .replace(f"\n{match.group(2)}", "\n")\
                   .replace(match.group(1), "")\
                   .replace(match.group(3), "")\
                   .replace("..ci:", "def ci_next(config, boards):")
        return run_ci(config, function)
    else:
        build = [b for b in list(build_command) if f":build:{b}" in config]
        return [(config, build, None)]


def run(where, command):
    global results
    result = subprocess.run(command, shell=True, cwd=where, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    results += result.returncode
    if result.returncode: print(result.stdout.decode("utf-8"));
    output = result.stderr.decode("utf-8").strip()
    if len(output): print("\n", output)
    return result.returncode

# =============================================================================
start_time_all = time.time()
projects = [p for path in sys.argv[1:] for p in Path(path).glob("**/project.xml")]
results = 0
num_builds = 0

for project in projects:
    raw_config = project.read_text()
    def clean(with_exit=False):
        cleanup(project.parent)
        if with_exit: exit(results);
    signal.signal(signal.SIGINT, lambda s, f: clean(True))

    for (config, builds, details) in config_parser(project, raw_config):
        print("=" * 78, "\nGenerating \"{}\"{} ... ".format(project.parent, f" with \"{details}\"" if details else ""), end="", flush=True)

        # Generate the library
        start_time = time.time()
        cleanup(project.parent)
        (project.parent / "project_ci.xml").write_text(config)
        lbuild = "lbuild -c project_ci.xml build --no-log " + " ".join(map("--module \"modm:build:{}\"".format, builds))
        result = run(project.parent, lbuild)
        if result: continue;
        print("done in {:=3.1f}s".format(time.time() - start_time))

        # Compile with the specific build systems
        for build in builds:
            print(f"Compiling with \"{build}\" ... ", end="", flush=True)
            start_time = time.time()
            result = run(project.parent, build_command[build])
            if not result: print("done in {:=3.1f}s".format(time.time() - start_time))
            num_builds += 1

        print("\n")
    clean()

print("{} builds in {:=3.0f}s".format(num_builds, time.time() - start_time_all))
exit(results)



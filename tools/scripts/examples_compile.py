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
import subprocess
import multiprocessing
from pathlib import Path

def run(where, command):
	result = subprocess.run(command, shell=True, cwd=where, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	output = ""
	if result.returncode:
		output += result.stdout.decode("utf-8").strip(" \n")
	output += result.stderr.decode("utf-8").strip(" \n")
	return (result.returncode, output)

def generate(project):
	path = project.parent
	output = ["=" * 90, "Generating: {}".format(path)]
	rc, ro = run(path, "lbuild build")
	print("\n".join(output + [ro]))
	return None if rc else project

def build(project):
	path = project.parent
	project_cfg = project.read_text()
	commands = []
	if ":build:scons" in project_cfg:
		commands.append("scons build")
	if ":build:cmake" in project_cfg:
		commands.append("make")

	rcs = 0
	for command in commands:
		output = ["=" * 90, "Building: {} with {}".format(path, "SCons" if "scons" in command else "CMake")]
		rc, ro = run(path, command)
		rcs += rc
		print("\n".join(output + [ro]))

	return None if rcs else project


cpus = 4 if os.getenv("CIRCLECI") else os.cpu_count()
print("Using {}x parallelism".format(cpus))
# Create build folder to prevent process race
(Path(__file__).parents[3] / "build").mkdir(exist_ok=True)
# Find all project files
projects = [p for path in sys.argv[1:] for p in Path(path).glob("**/project.xml")]
# first generate all projects
with multiprocessing.Pool(cpus) as pool:
    projects = pool.map(generate, projects)
results = projects.count(None)

# Filter projects for successful generation
projects = [p for p in projects if p is not None]
# Then build the successfully generated ones
with multiprocessing.Pool(cpus) as pool:
    projects = pool.map(build, projects)
results += projects.count(None)

exit(results)



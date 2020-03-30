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
import platform
import subprocess
import multiprocessing
from pathlib import Path

is_running_in_ci = os.getenv("CIRCLECI") is not None
cpus = 4 if is_running_in_ci else os.cpu_count()
build_dir = (Path(os.path.abspath(__file__)).parents[2] / "build")
cache_dir = build_dir / "cache"
global_options = {}
if is_running_in_ci:
	global_options["::build.path"] = "build/"
	global_options[":::cache_dir"] = str(cache_dir)

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
	options = " ".join("-D{}={}".format(k, v) for k,v in global_options.items())
	# Compile Linux examples under macOS with hosted-darwin target
	if "Darwin" in platform.platform() and "hosted-linux" in project.read_text():
		options += " -D:target=hosted-darwin"
	rc, ro = run(path, "lbuild {} build".format(options))
	print("\n".join(output + [ro]))
	return None if rc else project

def build(project):
	path = project.parent
	project_cfg = project.read_text()
	commands = []
	if ":build:scons" in project_cfg:
		commands.append("python3 `which scons` build --cache-show --random")
	if ":build:cmake" in project_cfg:
		commands.append("make cmake && make build")

	rcs = 0
	for command in commands:
		output = ["=" * 90, "Building: {} with {}".format(
		          path, "SCons" if "scons" in command else "CMake")]
		rc, ro = run(path, command)
		rcs += rc
		print("\n".join(output + [ro]))

	return None if rcs else project

def compile_examples(paths):
	print("Using {}x parallelism".format(cpus))
	# Create build folder to prevent process race
	cache_dir.mkdir(exist_ok=True, parents=True)
	(cache_dir / "config").write_text('{"prefix_len": 2}')
	# Find all project files
	projects = [p for path in paths for p in Path(path).glob("**/project.xml")]
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

	return results


if __name__ == '__main__':
	exit(compile_examples(sys.argv[1:]))
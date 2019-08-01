#!/usr/bin/env python3
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os
import fnmatch
import subprocess
import multiprocessing
from pathlib import Path

def repopath(path):
	return (Path(os.path.abspath(__file__)).parents[2] / path)

def run(where, command, stdin=None):
	print(command)
	result = subprocess.run(command, shell=True, cwd=where, input=stdin, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	return (result.returncode,
	        result.stdout.decode("utf-8").strip(" \n"),
	        result.stderr.decode("utf-8").strip(" \n"))

ignored = [
	".git*",
	"*.png",
	"*.svg",
	"*.css",
	"*.dot",
	"*.pbm",
	"*.font",
	"ext/arm/cmsis/CMSIS",
	"ext/dlr/scons-build-tools",
	"ext/fatfs*",
	"ext/aws/freertos*",
	"ext/adamgreen/crashcatcher",
	"ext/gcc/libstdc++",
	"ext/modm-devices",
	"ext/nxp*",
	"ext/ros/ros-lib",
	"ext/st/stm32*",
	"ext/tlsf*",
]

all_files = run(repopath("."), "git ls-files")[1].splitlines()
filtered = []
markdowns = []
for ffile in all_files:
	if fnmatch.fnmatch(ffile, "*.md*"):
		markdowns.append(ffile)
		continue
	if any(fnmatch.fnmatch(ffile, i) for i in ignored):
		continue
	filtered.append(ffile)

print("#Files: ", len(filtered))
# Remove all trailing whitespace for regular files
run(repopath("."), r"xargs -0 -n1 sed -i 's/\s*$//'", stdin="\0".join(filtered).encode("utf-8"))
# But for markdown files, only remove non-double space trailing whitespace
run(repopath("."), r"xargs -0 -n1 sed -i 's/(?!  $)\s*$//'", stdin="\0".join(markdowns).encode("utf-8"))

differences = run(repopath("."), r"git diff")[1]
if len(differences):
	subprocess.run("git --no-pager diff", shell=True, cwd=repopath("."))
	print("\n\nPlease remove all trailing whitespace!\n\n")
	exit(1)

exit(0)
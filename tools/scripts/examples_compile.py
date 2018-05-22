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
from pathlib import Path

def run(where, command):
	global results
	result = subprocess.run(command, shell=True, cwd=where, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	results += result.returncode
	if result.returncode: print(result.stdout.decode("utf-8"));
	print(result.stderr.decode("utf-8"))
	return result.returncode

projects = [p for path in sys.argv[1:] for p in Path(path).glob("**/project.xml")]
results = 0

for project in projects:
	print("============================================================================================")
	print("Building: {}".format(project.parent))
	result = run(project.parent, "lbuild build")
	if result: continue;
	if "build.scons" in project.read_text():
		run(project.parent, "scons build")
	elif "build.cmake" in project.read_text():
		run(project.parent, "make")

exit(results)



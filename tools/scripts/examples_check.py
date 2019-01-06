#!/usr/bin/env python3
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os, re, sys, subprocess
from pathlib import Path
from collections import defaultdict

def check_unique_paths(projects):
	# build paths *must* be unique, otherwise parallel compilation will overwrite build artifacts
	paths = defaultdict(list)
	for project in projects:
		match = re.search(r"<option name=\".*?:build.path\">(.*?)</option>", project.read_text())
		if match:
			match = match.group(1);
		else:
			match = "build"
		paths[match].append(project)

	result = 0
	for path, projects in paths.items():
		if path == "build":
			for project in projects:
				print("'{}' has no build path set!".format(project), file=sys.stderr)
				result += 1
		elif len(projects) > 1:
			print("The build path '{}' is used by these projects:".format(path), file=sys.stderr)
			for project in projects:
				print(" - '{}'".format(project), file=sys.stderr)
				result += 1

	return result


# Find all project files
projects = [p for path in sys.argv[1:] for p in Path(path).glob("**/project.xml")]

result = check_unique_paths(projects)

exit(result)



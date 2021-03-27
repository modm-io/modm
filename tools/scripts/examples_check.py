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

repopath = lambda path: Path(__file__).parents[2] / path
relpath = lambda path: os.path.relpath(path, repopath("."))

def check_builds_paths(projects):
	# Get all build paths
	paths = defaultdict(list)
	for project in projects:
		match = re.search(r"<option name=\".*?:build.path\">(.*?)</option>", project.read_text())
		if match:
			path = match.group(1)
			match = (project.parent / match.group(1)).resolve();
		else:
			path = "build"
			match = path
		paths[match].append( (project, path) )

	result = 0
	for path, projects in paths.items():
		# Check that all build paths are unique, otherwise parallel compilation will overwrite build artifacts
		if path == "build":
			for project, _ in projects:
				print("\nProject '{}' has no build path set!".format(relpath(project)), file=sys.stderr)
				result += 1
		# Check that all build paths are unique
		elif len(projects) > 1:
			print("\nThe build path '{}' is used by multiple projects:".format(path), file=sys.stderr)
			for project, bpath in projects:
				print(" - '{}': '{}'".format(relpath(project), bpath), file=sys.stderr)
				result += 1
		# Check that all build paths are actually inside the build folder
		if len(projects):
			relbuildpath = os.path.relpath(path, repopath("build"))
			if relbuildpath.startswith(".."):
				print("\nThe build path '{}' is outside of the 'modm/build' folder!"
				      .format(relpath(path)), file=sys.stderr)
				for project, bpath in projects:
					print(" - '{}': '{}'".format(relpath(project), bpath), file=sys.stderr)
				result += 1

	return result

def _get_paths_from_ci(files):
	paths = set()
	for file in files:
		matches = re.findall(r"examples_compile.py (.*?)\)", file.read_text())
		paths |= set(m  for match in matches  for m in match.split(" "))
	return paths

def check_is_part_of_ci(projects):
	folders = set(p.relative_to(repopath("examples")).parts[0] for p in projects)
	result = 0
	# Linux files
	paths = _get_paths_from_ci([repopath(".github/workflows/linux.yml")])
	paths = folders - paths - {'rpi'}
	if paths:
		print("\nLinux CI is missing examples: '{}'"
		      .format("', '".join(sorted(list(paths)))), file=sys.stderr)
		print("    Please add these example folders to the appropriate CI job in\n"
		      "    '.github/workflows/linux.yml'!")

	return len(paths)

if __name__ == "__main__":
	# Find all project files
	projects = list(repopath("examples").rglob("*/project.xml"))
	# Run a bunch of checks on them
	result = check_builds_paths(projects)
	result += check_is_part_of_ci(projects)
	# Return code if any
	exit(result)



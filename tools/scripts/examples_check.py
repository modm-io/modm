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

def check_ci_workflows(folders):
	def _check(path, missing=None):
		path = ".github/workflows/" + path
		matches = re.findall(r"examples_compile.py (.*?)\)", repopath(path).read_text())
		paths = set(m  for match in matches  for m in match.split(" "))

		if nonexistant_paths := paths - folders:
			print("\nThe CI is trying to build examples that don't exist:\n\n- " +
				  "\n- ".join(sorted(list(nonexistant_paths))) +
				  "\n\n  Please remove these example folders from the CI configuration:"
				 f"\n    '{path}'!", file=sys.stderr)

		if missing is not None and (missing_paths := folders - paths - missing):
			print("\nThe CI is missing examples:\n\n- " +
				  "\n- ".join(sorted(list(missing_paths))) +
				  "\n\n  Please add these example folders to the appropriate CI job in"
				 f"\n    '{path}'!", file=sys.stderr)
			return len(missing_paths) + len(nonexistant_paths)

		return len(nonexistant_paths)

	errors = _check("linux.yml", {'rpi'})
	errors += _check("macos.yml")
	errors += _check("windows.yml")
	return errors

if __name__ == "__main__":
	# Find all project files
	projects = list(repopath("examples").rglob("*/project.xml"))
	folders = set(p.relative_to(repopath("examples")).parts[0] for p in projects)
	# Run a bunch of checks on them
	result = check_builds_paths(projects)
	result += check_ci_workflows(folders)
	# Return code if any
	exit(result)



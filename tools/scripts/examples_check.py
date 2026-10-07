#!/usr/bin/env python3
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os, re, sys
from pathlib import Path
from examples_compile import configurations, targets

repopath = lambda path: Path(__file__).parents[2] / path
relpath = lambda path: os.path.relpath(path, repopath("."))

def check_configurations(projects):
	# All board configurations and their revisions known to lbuild
	boards = set()
	for module in repopath("src/modm/board").glob("*/module.lb"):
		module_text = module.read_text()
		name = "modm:" + re.search(r"\.name += +\".*?:board:(.*?)\"", module_text).group(1)
		versions = re.search(r"#+ +[Rr]evisions? += +\[(.*)\]", module_text)
		boards |= {name} | {f"{name}:{v.strip()}" for v in (versions.group(1).split(",") if versions else [])}

	boards |= {"modm:hosted"} | {f"modm:hosted:{c.stem}" for c in repopath("tools/devices/hosted/config").glob("*.xml")}

	result = 0
	used = set()
	for project in projects:
		configs = set(configurations(project.read_text()))
		used |= {":".join(c.split(":")[:2]) for c in configs}
		# A typo in a config would otherwise only fail when compiling this example
		if unknown := configs - boards:
			print("\nProject '{}' extends unknown configurations:\n\n- {}"
				  .format(relpath(project), "\n- ".join(sorted(unknown))), file=sys.stderr)
			result += len(unknown)

	# Every board must be compiled by at least one example
	if unused := {b for b in boards if b.count(":") == 1} - used:
		print("\nThese boards are not used by any example:\n\n- " + "\n- ".join(sorted(unused)) +
			  "\n\n  Please add them to 'examples/gpio/blinky/project.xml'!", file=sys.stderr)
		result += len(unused)

	return result

def check_ci_workflows(projects):
	# Every target must be compiled by one of the Linux CI jobs
	workflow = repopath(".github/workflows/linux.yml").read_text()
	patterns = re.findall(r"examples_compile.py .+? --target '(.+?)'", workflow)
	missing = {target for project in projects for target in targets(project)
			   if not any(re.search(p, target) for p in patterns)}
	if missing:
		print("\nThe CI does not compile examples for these targets:\n\n- " + "\n- ".join(sorted(missing)) +
			  "\n\n  Please add a job with a matching '--target' to '.github/workflows/linux.yml'!", file=sys.stderr)
	return len(missing)

if __name__ == "__main__":
	# Find all project files
	projects = [p for d in ("examples", "test/integration") for p in repopath(d).rglob("*/project.xml")]
	# Run a bunch of checks on them
	result = check_configurations(projects)
	result += check_ci_workflows(projects)
	# Return code if any
	exit(result)

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2019, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import shutil
from SCons.Script import *
import subprocess

try:
    from elftools.elf.elffile import ELFFile
except:
    print("elftools are missing, you need to `pip install pyelftools`!")
    exit(1)

def run_store_artifact(target, source, env):
	artifactpath = os.path.join(env["CONFIG_BUILD_BASE"], "artifacts")
	try:
		os.makedirs(artifactpath)
	except:
		pass

	with open(source[0].path, "rb") as src:
		elffile = ELFFile(src)
		build_id = elffile.get_section_by_name(".build_id")
		if build_id is not None:
			for note in build_id.iter_notes():
				if note['n_type'] == "NT_GNU_BUILD_ID":
					build_id = note['n_desc']

	if build_id is not None:
		artifact = os.path.join(artifactpath, "{}.elf".format(build_id.lower()))
		shutil.copy2(source[0].path, artifact)
	else:
		print("Unable to find Build ID for '{}'!".format(source[0].path))
	return 0

def store_artifact(env, source, alias="store_artifact"):
	action = Action(run_store_artifact, cmdstr="$ARTIFACTSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get("verbose"):
		env["ARTIFACTSTR"] = "{}Cache Artifact· {}$SOURCE{}" \
				.format("\033[;0;32m", "\033[;0;33m", "\033[;0;0m")

	env.AddMethod(store_artifact, "CacheArtifact")

def exists(env):
	return True

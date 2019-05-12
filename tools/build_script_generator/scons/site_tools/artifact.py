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
import binascii

def run_store_artifact(target, source, env):
	artifactpath = os.path.join(env["CONFIG_BUILD_BASE"], "artifacts")
	try:
		os.makedirs(artifactpath)
	except:
		pass
	source = str(source[0])
	binary = os.path.splitext(source)[0]+".bin"
	subprocess.call(env.subst("$OBJCOPY -O binary {} {}".format(source, binary)), shell=True)
	with open(binary, "rb") as binfile:
		firmware = binascii.crc32(binfile.read()) & 0xFFFFFFFF

	artifact = os.path.join(artifactpath, "{:08X}.elf".format(firmware))
	shutil.copy2(source, artifact)
	shutil.copy2(binary, os.path.splitext(artifact)[0]+".bin")
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

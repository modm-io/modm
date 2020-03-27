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

from SCons.Script import *
from modm_tools import build_id

def store_artifact(env, source, alias="store_artifact"):
	def run_store_artifact(target, source, env):
		build_id.cache_elf(source[0].path, env["CONFIG_ARTIFACT_PATH"])
		return 0
	action = Action(run_store_artifact, cmdstr="$ARTIFACT_STR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

def generate(env, **kw):
	if not ARGUMENTS.get("verbose"):
		env["ARTIFACT_STR"] = \
			"{0}.---Artifact--- {1}$SOURCE\n" \
			"{0}'----Cache----> {2}$CONFIG_ARTIFACT_PATH{3}" \
			.format("\033[;0;32m", "\033[;0;33m", "\033[;1;33m", "\033[;0;0m")

	env.AddMethod(store_artifact, "CacheArtifact")

def exists(env):
	return True

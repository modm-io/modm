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

import os.path
from SCons.Script import *
from modm_tools import build_id

def store_artifact(env, source, alias="store_artifact"):
	def run_store_artifact(target, source, env):
		build_id.cache_elf(source[0].path, env["CONFIG_ARTIFACT_PATH"])
		return 0
	def comstr_artifact(target, source, env):
		env["ARTIFACT_FILEPATH"] = "{}/{}.elf".format(
				os.path.relpath(env["CONFIG_ARTIFACT_PATH"]),
				build_id.extract(source[0].path))
		return env.subst(env["ARTIFACT_COMSTR"], source=source)
	action = Action(run_store_artifact, comstr_artifact)
	return env.AlwaysBuild(env.Alias(alias, source, action))

def generate(env, **kw):
	env.AddMethod(store_artifact, "CacheArtifact")

def exists(env):
	return True

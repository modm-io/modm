#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import platform
from SCons.Script import *
from modm_tools import avrdude

def call_avrdude(env, source, fuses=None):
	avrdude.program(source=source,
					device=env["CONFIG_AVRDUDE_DEVICE"],
					programmer=ARGUMENTS.get("programmer", env.get("CONFIG_AVRDUDE_PROGRAMMER")),
					baudrate=ARGUMENTS.get("baudrate", env.get("CONFIG_AVRDUDE_BAUDRATE")),
					port=ARGUMENTS.get("port", env.get("CONFIG_AVRDUDE_PORT", "auto")),
					options=env.get("CONFIG_AVRDUDE_OPTIONS"),
					fuses=fuses)

# -----------------------------------------------------------------------------
def avrdude_flash(env, source, alias="avrdude_flash"):
	def call_program(target, source, env):
		call_avrdude(env, source[0])

	action = Action(call_program, cmdstr="$PROGRAM_AVRDUDE_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

def avrdude_fuse(env, source, alias="avrdude_fuse"):
	def call_fuse(target, source, env):
		call_avrdude(env, source[0], fuses=["hfuse", "lfuse", "efuse"])

	action = Action(call_fuse, cmdstr="$PROGRAM_AVRDUDE_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(avrdude_flash, "ProgramAvrdude")
	env.AddMethod(avrdude_fuse, "ProgramAvrdudeFuses")

def exists(env):
	return env.Detect("avrdude")

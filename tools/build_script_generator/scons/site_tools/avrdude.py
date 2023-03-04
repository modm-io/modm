#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, 2023, Niklas Hauser
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
def avrdude_flash(env, source):
	def call_program(target, source, env):
		call_avrdude(env, source[0])
	return env.AlwaysBuildAction(call_program, "$PROGRAM_AVRDUDE_COMSTR", source)

def avrdude_fuse(env, source):
	def call_fuse(target, source, env):
		call_avrdude(env, source[0], fuses=["hfuse", "lfuse", "efuse"])
	return env.AlwaysBuildAction(call_fuse, "$PROGRAM_AVRDUDE_COMSTR", source)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(avrdude_flash, "ProgramAvrdude")
	env.AddMethod(avrdude_fuse, "ProgramAvrdudeFuses")

def exists(env):
	return env.Detect("avrdude")

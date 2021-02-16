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
from modm_tools import bossac

# -----------------------------------------------------------------------------
def bossac_program(env, source, alias="bossac_program"):
	def call_program(target, source, env):
		offset = env.get("MODM_BOSSAC_OFFSET", 0)
		if offset < 0x1000: # Is 4kB the smallest SAM-BA bootloader?
			raise ValueError("Refusing to overwrite bootloader!\n"
			                 "  Hint: Set `:platform:cortex-m:linkerscript.flash_offset` to bootloader size!")
		else:
			bossac.program(source=source[0], offset=offset,
						   port=ARGUMENTS.get("port", "auto"),
						   options=env.get("MODM_BOSSAC_OPTIONS"))

	action = Action(call_program, cmdstr="$PROGRAM_BOSSAC_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(bossac_program, "ProgramBossac")

def exists(env):
	return env.Detect("bossac")

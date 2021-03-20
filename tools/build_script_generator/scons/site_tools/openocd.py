#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2011-2012, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2016-2017, Niklas Hauser
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2016, Tarik TIRE
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import platform
from SCons.Script import *

from modm_tools import gdb, openocd
from modm_tools.openocd import OpenOcdBackend

# -----------------------------------------------------------------------------
def debug_openocd(env, source, alias="debug_openocd"):
	def call_debug_openocd(target, source, env):
		config_openocd = env.Listify(env.get("MODM_OPENOCD_CONFIGFILES", []))
		config_searchdirs = env.Listify(env.get("MODM_OPENOCD_SEARCHDIRS", []))
		backend = OpenOcdBackend(config=map(env.subst, config_openocd),
								 search=map(env.subst, config_searchdirs))
		config  = env.Listify(env.get("MODM_OPENOCD_GDBINIT", []))
		config += env.Listify(env.get("MODM_GDBINIT", []))
		commands = env.Listify(env.get("MODM_GDB_COMMANDS", []))
		gdb.call(source=source[0], backend=backend, config=map(env.subst, config),
				 commands=map(env.subst, commands), ui=ARGUMENTS.get("ui", "tui"))

	action = Action(call_debug_openocd, cmdstr="$DEBUG_OPENOCD_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def program_openocd(env, source, alias="program_openocd"):
	def call_program_openocd(target, source, env):
		config = env.Listify(env.get("MODM_OPENOCD_CONFIGFILES", []))
		searchdir = env.Listify(env.get("MODM_OPENOCD_SEARCHDIRS", []))
		openocd.program(source=str(source[0]),
						config=map(env.subst, config),
						search=map(env.subst, searchdir))

	action = Action(call_program_openocd, cmdstr="$PROGRAM_OPENOCD_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def reset_openocd(env, alias="reset_openocd"):
	def call_reset_openocd(target, source, env):
		config = env.Listify(env.get("MODM_OPENOCD_CONFIGFILES", []))
		searchdir = env.Listify(env.get("MODM_OPENOCD_SEARCHDIRS", []))
		openocd.reset(config=map(env.subst, config),
		              search=map(env.subst, searchdir))

	action = Action(call_reset_openocd, cmdstr="$RESET_OPENOCD_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, '', action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(program_openocd, "ProgramOpenOcd")
	env.AddMethod(debug_openocd, "DebugOpenOcd")
	env.AddMethod(reset_openocd, "ResetOpenOcd")

def exists(env):
	return env.Detect("openocd")

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, Sascha Schade
# Copyright (c) 2014-2016, Daniel Krebs
# Copyright (c) 2015, Georgi Grinshpun
# Copyright (c) 2016, Niklas Hauser
# Copyright (c) 2017, Michael Thies
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import platform
from SCons.Script import *
from modm_tools import gdb, openocd, backend as bem

# -----------------------------------------------------------------------------
# Program elf file via a remote gdb session
def program_openocd_gdb_remote(env, source, alias='program_openocd_gdb_remote'):
	def call_remote_program(target, source, env):
		host   = ARGUMENTS.get("host", env.get("OPENOCD_REMOTE_HOST", None))
		config = env.Listify(env.get("MODM_OPENOCD_GDBINIT", []))
		gdb.call(source=source[0], backend=bem.ExtendedRemote(host),
		         config=map(env.subst, config),
		         commands=["load", "monitor reset", "disconnect", "quit"])

	action = Action(call_remote_program, cmdstr="$PROGRAM_REMOTE_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
# Interactively debug via a remote gdb session
def debug_openocd_gdb_remote(env, source, alias='debug_openocd_gdb_remote'):
	def call_remote_debug(target, source, env):
		host    = ARGUMENTS.get("host", env.get("OPENOCD_REMOTE_HOST", None))
		config  = env.Listify(env.get("MODM_OPENOCD_GDBINIT", []))
		config += env.Listify(env.get("MODM_GDBINIT", []))
		gdb.call(source=source[0], backend=bem.ExtendedRemote(host),
				 config=map(env.subst, config), ui=ARGUMENTS.get("ui", "tui"),)

	action = Action(call_remote_debug, cmdstr="$DEBUG_REMOTE_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
# Reset processor via remote gdb session
def reset_openocd_gdb_remote(env, alias='reset_openocd_gdb_remote'):
	def call_remote_reset(target, source, env):
		host   = ARGUMENTS.get("host", env.get("OPENOCD_REMOTE_HOST", None))
		config = env.Listify(env.get("MODM_OPENOCD_GDBINIT", []))
		gdb.call(backend=bem.ExtendedRemote(host), config=map(env.subst, config),
		         commands=["monitor reset", "disconnect", "quit"])

	action = Action(call_remote_reset, cmdstr="$RESET_REMOTE_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, '', action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(program_openocd_gdb_remote, 'ProgramGdbRemote')
	env.AddMethod(debug_openocd_gdb_remote,   'DebugGdbRemote')
	env.AddMethod(reset_openocd_gdb_remote,   'ResetGdbRemote')

def exists(env):
	return env.Detect('arm-none-eabi-gdb')

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, Sascha Schade
# Copyright (c) 2014-2016, Daniel Krebs
# Copyright (c) 2015, Georgi Grinshpun
# Copyright (c) 2016, 2023, Niklas Hauser
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
from modm_tools import gdb
from modm_tools.backend import DebugBackend

# -----------------------------------------------------------------------------
def _config(env):
	gconfig  = env.SubstList("MODM_GDBINIT")
	gcmds = env.SubstList("MODM_GDB_COMMANDS")
	return (gconfig, gcmds)

# -----------------------------------------------------------------------------
def program_gdb_remote(env, source):
	def call_remote_program(target, source, env):
		host = ARGUMENTS.get("host", env.get("GDB_REMOTE_HOST", ":3333"))
		gconfig, _ = _config(env)
		gdb.call(DebugBackend(host), source=source[0], config=gconfig,
		         commands=["load", "monitor reset", "disconnect", "quit"])
	return env.AlwaysBuildAction(call_remote_program, "$PROGRAM_REMOTE_COMSTR", source)

# -----------------------------------------------------------------------------
def debug_gdb_remote(env, source):
	def call_remote_debug(target, source, env):
		host = ARGUMENTS.get("host", env.get("GDB_REMOTE_HOST", ":3333"))
		gconfig, gcmds = _config(env)
		gdb.call(DebugBackend(host), source=source[0], config=gconfig,
		         commands=gcmds, ui=ARGUMENTS.get("ui", "tui"))
	return env.AlwaysBuildAction(call_remote_debug, "$DEBUG_REMOTE_COMSTR", source)

# -----------------------------------------------------------------------------
def reset_gdb_remote(env):
	def call_remote_reset(target, source, env):
		host   = ARGUMENTS.get("host", env.get("GDB_REMOTE_HOST", ":3333"))
		gconfig, _ = _config(env)
		gdb.call(DebugBackend(host), config=gconfig,
		         commands=["monitor reset", "disconnect", "quit"])
	return env.AlwaysBuildAction(call_remote_reset, "$RESET_REMOTE_COMSTR")

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(program_gdb_remote, 'ProgramGdbRemote')
	env.AddMethod(debug_gdb_remote,   'DebugGdbRemote')
	env.AddMethod(reset_gdb_remote,   'ResetGdbRemote')

def exists(env):
	return env.Detect('arm-none-eabi-gdb')

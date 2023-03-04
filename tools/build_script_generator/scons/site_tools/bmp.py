#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, Sascha Schade
# Copyright (c) 2017, Michael Thies
# Copyright (c) 2023, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import platform
from SCons.Script import *
from modm_tools import gdb, bmp

# -----------------------------------------------------------------------------
def _config(env):
	gconfig  = env.SubstList("MODM_GDBINIT_BMP", "MODM_GDBINIT")
	gcmds = env.SubstList("MODM_GDB_COMMANDS")
	return (gconfig, gcmds)

# -----------------------------------------------------------------------------
def black_magic_probe_debug(env, source):
	def call_bmp_debug(target, source, env):
		backend = bmp.BlackMagicProbeBackend(port=ARGUMENTS.get("port", "auto"))
		gconfig, gcmds = _config(env)
		gdb.call(backend, source=source[0].abspath, config=gconfig,
			 	 commands=gcmds, ui=ARGUMENTS.get("ui", "tui"))
	return env.AlwaysBuildAction(call_bmp_debug, "$DEBUG_BMP_COMSTR", source)

def black_magic_probe_coredump(env):
	def call_bmp_coredump(target, source, env):
		backend = bmp.BlackMagicProbeBackend(port=ARGUMENTS.get("port", "auto"))
		gconfig, gcmds = _config(env)
		gcmds += ["modm_coredump", "modm_build_id", "quit"]
		gdb.call(backend, config=gconfig, commands=gcmds)
	return env.AlwaysBuildAction(call_bmp_coredump, "$DEBUG_COREDUMP_COMSTR")

# -----------------------------------------------------------------------------
def black_magic_probe_program(env, source):
	def call_bmp_program(target, source, env):
		bmp.program(ARGUMENTS.get("port", "auto"), source[0].abspath)
	return env.AlwaysBuildAction(call_bmp_program, "$PROGRAM_BMP_COMSTR", source)

def black_magic_probe_reset(env):
	def call_bmp_reset(target, source, env):
		bmp.reset(ARGUMENTS.get("port", "auto"))
	return env.AlwaysBuildAction(call_bmp_reset, "$RESET_BMP_COMSTR")

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(black_magic_probe_program, 'ProgramBMP')
	env.AddMethod(black_magic_probe_debug, 'DebugBMP')
	env.AddMethod(black_magic_probe_reset, 'ResetBMP')
	env.AddMethod(black_magic_probe_coredump, 'CoredumpBMP')

def exists(env):
	return True

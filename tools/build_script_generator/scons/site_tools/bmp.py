#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, Sascha Schade
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
from modm_tools import gdb, bmp

# -----------------------------------------------------------------------------
def black_magic_probe_program(env, source, alias='black_magic_probe_program'):
	def call_bmp_program(target, source, env):
		bmp.program(source=source[0].abspath, port=ARGUMENTS.get("port", "auto"))

	action = Action(call_bmp_program, cmdstr="$BMP_PROGRAM_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def black_magic_probe_debug(env, source, alias='black_magic_probe_debug'):
	def call_bmp_debug(target, source, env):
		backend = bmp.BlackMagicProbeBackend(port=ARGUMENTS.get("port", "auto"))
		gdb.call(source=source[0].abspath, backend=backend, ui=ARGUMENTS.get("ui", "tui"),
			 	 config=map(env.subst, env.Listify(env.get("MODM_GDBINIT", []))),
			 	 commands=map(env.subst, env.Listify(env.get("MODM_GDB_COMMANDS", []))))

	action = Action(call_bmp_debug, cmdstr="$BMP_DEBUG_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def black_magic_probe_reset(env, alias='black_magic_probe_reset'):
	def call_bmp_reset(target, source, env):
		bmp.reset(port=ARGUMENTS.get("port", "auto"))

	action = Action(call_bmp_reset, cmdstr="$BMP_RESET_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, '', action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(black_magic_probe_program, 'ProgramBMP')
	env.AddMethod(black_magic_probe_debug, 'DebugBMP')
	env.AddMethod(black_magic_probe_reset, 'ResetBMP')

def exists(env):
	return True

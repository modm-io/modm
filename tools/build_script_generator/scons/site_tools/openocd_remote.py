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

# -----------------------------------------------------------------------------
# Copy the hex file to the remote target as /tmp/openocd.hex
# Then use telnet interface of openocd to remotely control openocd to flash
# hex file to connected target.
# openocd must be running on target at port 4444
def openocd_remote_run(env, source, alias='openocd_remote_run'):
	if platform.system() == "Windows":

		def fail(target, source, env):
			raise Exception("Not supported under windows")

		action = fail
		return env.AlwaysBuild(env.Alias(alias, source, action))
	else:
		commands = ["init", "reset halt", "flash write_image erase /tmp/openocd.hex", "reset halt", "mww 0xE000EDF0 0xA05F0000"]
		action = Action("scp $SOURCE $OPENOCD_REMOTE_USER@$OPENOCD_REMOTE_HOST:/tmp/openocd.hex; echo %s | nc $OPENOCD_REMOTE_HOST 4444" % ' '.join(['"%s;"' % c for c in commands]),
			cmdstr="$OPENOCD_COMSTR")
		return env.AlwaysBuild(env.Alias(alias, source, action))
# -----------------------------------------------------------------------------
# Program elf file via a remote gdb session
def gdb_remote_program(env, source, alias='gdb_remote_program'):
	gdb = "arm-none-eabi-gdb"
	cmd = [gdb, '-q',
		'-ex "target remote $OPENOCD_REMOTE_HOST:3333"',
		'-ex "monitor reset halt"',
		'-ex "load"',
		'-ex "monitor reset "',
		'-ex "disconnect"',
		'-ex "quit"',
		'$SOURCE']

	action = Action(' '.join(cmd))
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
# Interactively debug via a remote gdb session
def gdb_remote_debug(env, source, alias='gdb_remote_debug'):
	gdb = "arm-none-eabi-gdb"
	cmd = [gdb, '-q',
	    '--tui',
		'-ex "target remote $OPENOCD_REMOTE_HOST:3333"',
		'-ex "monitor halt"',
		'$SOURCE']

	action = Action(' '.join(cmd))
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
# Reset processor via remote gdb session
def gdb_remote_reset(env, alias='gdb_remote_reset'):
	if platform.system() == "Windows":

		def fail(target, source, env):
			raise Exception("Not supported under windows")

		action = fail
		return env.AlwaysBuild(env.Alias(alias, '', action))
	else:
		gdb = "arm-none-eabi-gdb"
		cmd = [gdb, '-q',
			'-ex "target remote $OPENOCD_REMOTE_HOST:3333"',
			'-ex "monitor reset"',
			'-ex "disconnect"',
			'-ex "quit"']

		action = Action(' '.join(cmd))
		return env.AlwaysBuild(env.Alias(alias, '', action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get('verbose'):
		env['OPENOCD_COMSTR'] = "OpenOCD remote: program $SOURCE"

	env['OPENOCD'] = 'openocd'

	env.AddMethod(openocd_remote_run,  'OpenOcdRemote')
	env.AddMethod(gdb_remote_program,  'GdbRemoteProgram')
	env.AddMethod(gdb_remote_reset,    'GdbRemoteReset')
	env.AddMethod(gdb_remote_debug,    'GdbRemoteDebug')

def exists(env):
	return env.Detect('openocd_remote')

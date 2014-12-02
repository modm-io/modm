#!/usr/bin/env python
# 
# Copyright (c) 2014, Roboterclub Aachen e.V.
# All Rights Reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.

import platform
from SCons.Script import *

# -----------------------------------------------------------------------------
# Copy the hex file to the remote target as /tmp/openocd.hex
# Then use telnet interface of openocd to remotely control openocd to flash
# hex file to connected target.
# openocd must be running on target at port 4444
def openocd_remote_run(env, source, alias='openocd_remote_run'):
	if platform.system() == "Windows":
		print "Not supported under windows"
		exit(1)
	else:
		commands = ["init", "reset halt", "flash write_image erase /tmp/openocd.hex", "reset run"]
		action = Action("scp $SOURCE $OPENOCD_REMOTE_USER@$OPENOCD_REMOTE_HOST:/tmp/openocd.hex; echo %s | nc $OPENOCD_REMOTE_HOST 4444" % ' '.join(['"%s;"' % c for c in commands]),
			cmdstr="$OPENOCD_COMSTR")
		return env.AlwaysBuild(env.Alias(alias, source, action))
# -----------------------------------------------------------------------------
# Program elf file via a remote gdb session
def gdb_remote_program(env, source, alias='gdb_remote_program'):
	if platform.system() == "Windows":
		print "Not supported under windows"
		exit(1)
	else:
		gdb = "arm-none-eabi-gdb"
		cmd = [gdb, '-q',
			'-ex "target remote $OPENOCD_REMOTE_HOST:3333"',
			'-ex "load"',
			'-ex "monitor reset"',
			'-ex "disconnect"',
			'-ex "quit"',
			'$SOURCE']

		action = Action(' '.join(cmd))
		return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get('verbose'):
		env['OPENOCD_COMSTR'] = "OpenOCD remote: program $SOURCE"
	
	env['OPENOCD'] = 'openocd'

	env.AddMethod(openocd_remote_run,  'OpenOcdRemote')
	env.AddMethod(gdb_remote_program,  'GdbRemoteProgram')

def exists(env):
	return env.Detect('openocd_remote')

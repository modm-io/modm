#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2011-2012, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2016, Niklas Hauser
# Copyright (c) 2016, Daniel Krebs
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

# -----------------------------------------------------------------------------
def openocd_run(env, source, alias='openocd_run'):
	# The commands are wrapped into '-c "command1" -c "command2" ...'
	if env['OPENOCD_COMMANDS'] == 'default':
		env['OPENOCD_COMMANDS'] = """
			init
			reset halt
			flash write_image erase $SOURCE
			reset halt
			mww 0xE000EDF0 0xA05F0000
			shutdown"""

	if platform.system() == "Windows":
		env['OPENOCD_COMMANDS'] = env['OPENOCD_COMMANDS'].replace("$SOURCE", str(source[0]).replace("\\","/"))

	# Provide additional search paths via the OPENOCD_SCRIPTS environment variable
	# See http://openocd.org/doc/html/Running.html
	env['ENV']['OPENOCD_SCRIPTS'] = os.environ.get('OPENOCD_SCRIPTS')

	searchdir = os.path.abspath(os.path.join(env['XPCC_ROOTPATH'], "tools", "openocd"))
	commands = [c for c in env['OPENOCD_COMMANDS'].split('\n') if c != '']
	action = Action("$OPENOCD -f $OPENOCD_CONFIGFILE -s %s %s" % (searchdir, ' '.join(['-c "%s"' % c for c in commands])),
		cmdstr="$OPENOCD_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

def openocd_debug(env, source, alias='openocd_debug'):
	env['OPENOCD_COMMANDS'] = "init\nhalt"
	return openocd_run(env, source, alias)
	
# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get('verbose'):
		env['OPENOCD_COMSTR'] = "OpenOCD: program $SOURCE"
	
	env['OPENOCD'] = 'openocd'
	
	env.AddMethod(openocd_run, 'OpenOcd')
	env.AddMethod(openocd_debug, 'OpenOcdDebug')

def exists(env):
	return env.Detect('openocd')

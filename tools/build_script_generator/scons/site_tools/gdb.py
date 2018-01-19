#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2016-2017, Niklas Hauser
# Copyright (c) 2016, Daniel Krebs
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
# Start the gnu debugger
# -----------------------------------------------------------------------------

def gdb_debug(env, source, alias='gdb_debug'):
	actionString = '$GDB $SOURCE -ex "target extended-remote :$GDB_PORT"'
	action = Action(actionString, cmdstr="$GDB_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get('verbose'):
		env['GDB_COMSTR'] = "GDB: debugging $SOURCE"

	env['GDB'] = "arm-none-eabi-gdb -tui"
	env['GDB_PORT'] = '3333'

	env.AddMethod(gdb_debug, 'GdbDebug')

def exists(env):
	return env.Detect('gdb')

#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Georgi Grinshpun
# Copyright (c) 2012, Niklas Hauser
# Copyright (c) 2016, Daniel Krebs
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
def stlink_run(env, source, alias='stlink_run'):
	action = Action("$STLINK -c SWD UR -P $SOURCE -V -Rst", cmdstr="$STLINK_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get('verbose'):
		env['STLINK_COMSTR'] = "STLink: program $SOURCE"
	
#	if platform.system() == "Windows":
	env['STLINK'] = 'ST-LINK_CLI'
	
	env.AddMethod(stlink_run, 'STLink')

def exists(env):
	return True

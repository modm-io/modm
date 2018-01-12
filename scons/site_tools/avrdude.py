#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2014, 2016, Niklas Hauser
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
def avrdude_flash(env, source, eeprom_source='', alias='avrdude_program'):
	actionString = '$AVRDUDE -V -p $AVRDUDE_DEVICE -c $AVRDUDE_PROGRAMMER -P $AVRDUDE_PORT $AVRDUDE_OPTIONS -U flash:w:'
	if platform.system() == "Windows":
		# avrdude on Windows has problems with absolute path names.
		# The leading drive letter plus colon backslash (e.g. "c:\path")
		# gets confused with the colon used as argument separator.
		#
		# To avoid this problem we try to use relative path names if
		# possible.

		filename = str(source[0])
		if os.path.isabs(filename):
			filename = os.path.relpath(filename)
		filename = filename.replace("\\", "/")
		actionString += filename

		if (eeprom_source != ''):
			filename = str(eeprom_source[0])
			if os.path.isabs(filename):
				filename = os.path.relpath(filename)
			filename = filename.replace("\\", "/")
			actionString += "-U eeprom:w:" + filename

		if env.get('AVRDUDE_BAUDRATE') != []:
			actionString += " -b $AVRDUDE_BAUDRATE"

		action = Action(actionString, cmdstr="$AVRDUDE_COMSTR")
		return env.AlwaysBuild(env.Alias(alias, source, action))
	else:
		actionString += "$SOURCE"
		if (eeprom_source != ''):
			actionString += " -U eeprom:w:" + str(eeprom_source[0])

		if env.get('AVRDUDE_BAUDRATE') != []:
			actionString += " -b $AVRDUDE_BAUDRATE"

		action = Action(actionString, cmdstr="$AVRDUDE_COMSTR")
		return env.AlwaysBuild(env.Alias(alias, source, action))

def avrdude_fuse(env, alias='avrdude_fuse'):
	fusebits = []
	for fusebit in env['AVR_FUSEBITS']:
		key, value = next(iter(fusebit.items()))
		fusebits.append("-U %s:w:0x%02x:m" % (key, int(value, 0)))
	actionString = "$AVRDUDE -p $AVRDUDE_DEVICE -c $AVRDUDE_PROGRAMMER -P $AVRDUDE_PORT $AVRDUDE_OPTIONS -u %s" % " ".join(fusebits)

	if env.get('AVRDUDE_BAUDRATE') != []:
			actionString += " -b $AVRDUDE_BAUDRATE"

	action = Action(actionString,
					cmdstr="$AVRDUDE_FUSECOMSTR")
	return env.AlwaysBuild(env.Alias(alias, [], action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if ARGUMENTS.get('verbose') != '1':
		env['AVRDUDE_COMSTR'] = "avrdude: program $SOURCE"
		env['AVRDUDE_FUSECOMSTR'] = "avrdude: set fusebits"

	env['AVRDUDE'] = 'avrdude'

	env.AddMethod(avrdude_flash, 'Avrdude')
	env.AddMethod(avrdude_fuse, 'AvrdudeFuses')

def exists(env):
	return env.Detect('avrdude')

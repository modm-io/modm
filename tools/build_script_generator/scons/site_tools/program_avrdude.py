#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, Niklas Hauser
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
def avrdude_flash(env, source, eeprom_source="", alias="avrdude_program"):
	actionString = "$AVRDUDE -V -p $CONFIG_AVRDUDE_DEVICE -c $CONFIG_AVRDUDE_PROGRAMMER -P $CONFIG_AVRDUDE_PORT $CONFIG_AVRDUDE_OPTIONS -U flash:w:"
	if platform.system() == "Windows":
		# avrdude on Windows has problems with absolute path names.
		# The leading drive letter plus colon backslash (e.g. "c:\path")
		# gets confused with the colon used as argument separator.
		#
		# To avoid this problem we try to use relative path names if
		# possible.
		def relpath(path):
			if os.path.isabs(filename):
				filename = os.path.relpath(filename)
			return filename.replace("\\", "/")

		actionString += relpath(str(source[0]))
		if (eeprom_source != ""):
			eeprom_source = relpath(str(eeprom_source[0]))
	else:
		actionString += "$SOURCE"
		if (eeprom_source != ""):
			eeprom_source = str(eeprom_source[0])

	if (len(eeprom_source)):
		actionString += " -U eeprom:w:" + eeprom_source
	if env.get("CONFIG_AVRDUDE_BAUDRATE", False):
		actionString += " -b $CONFIG_AVRDUDE_BAUDRATE"

	action = Action(actionString, cmdstr="$AVRDUDE_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

def avrdude_fuse(env, alias="avrdude_fuse"):
	fusebits = ["-U {}:w:0x{:02x}:m".format(key, int(value, 0)) for key, value in env["CONFIG_AVR_FUSEBITS"].items()]
	actionString = "$AVRDUDE -p $AVRDUDE_DEVICE -c $CONFIG_AVRDUDE_PROGRAMMER -P $CONFIG_AVRDUDE_PORT $CONFIG_AVRDUDE_OPTIONS -u {}".format(" ".join(fusebits))

	if env.get("CONFIG_AVRDUDE_BAUDRATE", False):
		actionString += " -b $CONFIG_AVRDUDE_BAUDRATE"

	action = Action(actionString, cmdstr="$AVRDUDE_FUSECOMSTR")
	return env.AlwaysBuild(env.Alias(alias, [], action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if ARGUMENTS.get("verbose") != "1":
		env["AVRDUDE_COMSTR"] = "avrdude: program $SOURCE"
		env["AVRDUDE_FUSECOMSTR"] = "avrdude: set fusebits"

	env["AVRDUDE"] = "avrdude"

	env.AddMethod(avrdude_flash, "Avrdude")
	env.AddMethod(avrdude_fuse, "AvrdudeFuses")

def exists(env):
	return env.Detect("avrdude")

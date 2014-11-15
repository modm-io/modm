#!/usr/bin/env python
#
# Copyright (c) 2009, Roboterclub Aachen e.V.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Roboterclub Aachen e.V. nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import platform
from SCons.Script import *

# -----------------------------------------------------------------------------
def avrdude_flash(env, source, eeprom_source='', alias='avrdude_program'):
	actionString = '$AVRDUDE -p $AVRDUDE_DEVICE -c $AVRDUDE_PROGRAMMER -P $AVRDUDE_PORT $AVRDUDE_OPTIONS -U flash:w:'
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
		key, value = fusebit.items()[0]
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

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Georgi Grinshpun
# Copyright (c) 2012-2013, Sascha Schade
# Copyright (c) 2012-2013, 2016, Niklas Hauser
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import platform
import glob
from SCons.Script import *

# -----------------------------------------------------------------------------
# Support for the NXP/Code Red LPC-Link Debugger. 
# The LPCXpresso toolchain is needed for the firmware for the device.
# -----------------------------------------------------------------------------

def lpclink_init(env, source, alias='lpclink_init'):
	actionString = '$LPCDFU -d 0x471:0xdf55 -c 0 -t 2048 -R -D''$LPCLINK_FIRMWARE'
	action = Action(actionString, cmdstr="$LPCDFU_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

def lpclink_flash(env, source, alias='lpclink_flash'):
	actionString = '$LPCLINK -wire=winusb -p$CPU_STRING -flash-load-exec=$SOURCE -load-base=0x0'
	action = Action(actionString, cmdstr="$LPCLINK_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

def lpclink_debug(env, source, alias='lpclink_debug'):
	actionString = '$LPCLINK -wire=winusb -p$CPU_STRING -vendor=NXP -server=:$GDB_PORT'
	action = Action(actionString, cmdstr="$LPCLINK_DEBUG_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get('verbose'):
		env['LPCDFU_COMSTR']  		= "LPC-Link: writing firmware $LPCLINK_FIRMWARE with $LPCDFU"
		env['LPCLINK_COMSTR'] 		= "LPC-Link: program $SOURCE"
		env['LPCLINK_DEBUG_COMSTR'] = "LPC-Link: starting debugger"

	if env['LPCLINK_PATH'] == 'default':
		basePath = '/opt/lpcxpresso/'
		if platform.system() == 'Darwin':
			basePath = glob.glob('/Applications/lpcxpresso_*/')
			if basePath != []:
				basePath = basePath[0]
			else:
				basePath = ""
	else:
		basePath = env['LPCLINK_PATH']

	env['LPCDFU'] = basePath + 'lpcxpresso/bin/dfu-util'
	env['LPCLINK_FIRMWARE'] = basePath + 'lpcxpresso/bin/LPCXpressoWIN.enc'
	env['LPCLINK'] = basePath + 'lpcxpresso/bin/crt_emu_lpc11_13_nxp'
	env['GDB_PORT'] = '3333'

	# CPU for LPCLINK must be upper-case and underscore must be replace by slash
	env['CPU_STRING'] = env['ARM_DEVICE'].upper().replace("_","/")
	
	env.AddMethod(lpclink_init, 'LpcLinkInit')
	env.AddMethod(lpclink_flash, 'LpcLinkFlash')
	env.AddMethod(lpclink_debug, 'LpcLinkDebug')

def exists(env):
	return env.Detect('lpclink')

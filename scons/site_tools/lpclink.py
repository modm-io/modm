#!/usr/bin/env python
# 
# Copyright (c) 2012, Roboterclub Aachen e.V.
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
			
	basePath = '/opt/lpcxpresso/'
	if platform.system() == 'Darwin':
		basePath = glob.glob('/Applications/lpcxpresso_*/')[0]
	
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

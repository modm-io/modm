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
# 
# $Id$

import platform
from SCons.Script import *

# -----------------------------------------------------------------------------
def openocd_run(env, source, alias='openocd_run'):
	if platform.system() == "Windows":

		# The commands are wrapped into '-c "command1" -c "command2" ...'
		env['OPENOCD_COMMANDS'] = env['OPENOCD_COMMANDS'].replace("$SOURCE", str(source[0]).replace("\\","/"))
		commands = [c for c in env['OPENOCD_COMMANDS'].split('\n') if c != '']
#		commands[0] = commands[0].replace("\\", "/")
		action = Action("$OPENOCD -f $OPENOCD_CONFIGFILE %s" % ' '.join(['-c "%s"' % c for c in commands]), 
			cmdstr="$OPENOCD_COMSTR")
		return env.AlwaysBuild(env.Alias(alias, source, action))
	else:
	
		# The commands are wrapped into '-c "command1" -c "command2" ...'
		commands = [c for c in env['OPENOCD_COMMANDS'].split('\n') if c != '']
		action = Action("$OPENOCD -f $OPENOCD_CONFIGFILE %s" % ' '.join(['-c "%s"' % c for c in commands]), 
			cmdstr="$OPENOCD_COMSTR")
		return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if ARGUMENTS.get('verbose') != '1':
		env['OPENOCD_COMSTR'] = "OpenOCD: program $SOURCE"
	
	env['OPENOCD'] = 'openocd'
	
	env.AddMethod(openocd_run, 'OpenOcd')

def exists(env):
	return env.Detect('openocd')

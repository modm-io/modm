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

import os
import SCons

# -----------------------------------------------------------------------------
def generate(env, **kw):
	builder_packets = SCons.Script.Builder(
		action = SCons.Action.Action(
			'python "${XPCC_SYSTEM_BUILDER}/cpp_packets.py" ' \
				'--source_path ${TARGETS[0].dir} ' \
				'--header_path ${TARGETS[1].dir} ' \
				'$SOURCE',
			cmdstr="$SYSTEM_CPP_PACKETS_COMSTR"),
		emitter = \
			lambda target, source, env:
				([os.path.join(env['XPCC_BUILDPATH'], "robot/packets.cpp"),
				  os.path.join(str(source[0].dir), "robot/packets.hpp")],
				source),
		single_source = True,
		target_factory = env.fs.Entry,
		src_suffix = ".xml",
	)
	
	builder_identifier = SCons.Script.Builder(
		action = SCons.Action.Action(
			'python "${XPCC_SYSTEM_BUILDER}/cpp_identifier.py" ' \
				'--outpath ${TARGET.dir} ' \
				'$SOURCE',
			cmdstr="$SYSTEM_CPP_IDENTIFIER_COMSTR"),
		emitter = lambda target, source, env:
			([os.path.join(str(source[0].dir), "robot/identifier.hpp")], source),
		single_source = True,
		target_factory = env.fs.Entry,
		src_suffix = ".xml",
	)
	
	if SCons.Script.ARGUMENTS.get('verbose') != '1':
		env['SYSTEM_CPP_PACKETS_COMSTR'] = "Generate packets from: $SOURCE"
		env['SYSTEM_CPP_IDENTIFIER_COMSTR'] = "Generate identifier from: $SOURCE"
	
	env.Append(
		BUILDERS = {
			'SystemCppPackets': builder_packets,
			'SystemCppIdentifier': builder_identifier
		}
	)

def exists(env):
	return True

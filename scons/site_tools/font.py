#!/usr/bin/env python3
# 
# Copyright (c) 2011, Roboterclub Aachen e.V.
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

import os
import os.path

script_path = os.path.dirname(__file__)
command = os.path.join(script_path, "../../tools/font_creator/font_export.py")

# -----------------------------------------------------------------------------
def font_action(target, source, env):
	infile  = str(source[0])
	outfile = os.path.splitext(str(target[0]))[0]

	os.system('python3 "%s" "%s" "%s"' % (command, infile, outfile))

def font_string(target, source, env):
	return "Create Font: '%s'" % (str(source[0]))

def font_emitter(target, source, env):
	header = os.path.splitext(str(target[0]))[0] + ".hpp"
	target.append(header)
	env.Depends(target, command)
	return (target, source)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.Append(
		BUILDERS = {
			'Font': env.Builder(
				action = env.Action(font_action, font_string),
				suffix = '.cpp',
				src_suffix = '.font',
				emitter = font_emitter,
				single_source = True),
	})

def exists(env):
	return True

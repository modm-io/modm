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

from string import Template

from SCons.Script import *

# -----------------------------------------------------------------------------
def replace_action(target, source, env):
	if not env.has_key('SUBSTITUTIONS'):
		raise SCons.Errors.UserError, "'Template' requires SUBSTITUTIONS to be set."
	
	source = str(source[0])
	target = str(target[0])
	
	output = Template(open(source, 'r').read()).safe_substitute(env['SUBSTITUTIONS'])
	
	open(target, 'w').write(output)
	return 0

def replace_emitter(target, source, env):
	Depends(target, SCons.Node.Python.Value(env['SUBSTITUTIONS']))
	return target, source

def replace_string(target, source, env):
	return "Create: '%s' from '%s'" % (str(target[0]), str(source[0]))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	
	builder = env.Builder(
		action = env.Action(replace_action, replace_string),
		emitter = replace_emitter,
		src_suffix = '.in',
		single_source = True
	)
	
	env['BUILDERS']['Template'] = builder

def exists(env):
	return True

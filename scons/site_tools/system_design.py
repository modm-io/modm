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

import os
import re
import SCons
import SCons.Errors

# TODO make this more robust against whitespace etc.
includeExpression = re.compile(r'<include>(\S+)</include>', re.M)

# -----------------------------------------------------------------------------
def find_includes(file):
	""" Find include directives in an XML file """
	files = []
	for line in open(file).readlines():
		match = includeExpression.search(line)
		if match:
			filename = match.group(1)
			if not os.path.isabs(filename):
				filename = os.path.join(os.path.dirname(os.path.abspath(file)), filename)
			files.append(filename)
	return files

def xml_include_scanner(node, env, path, arg=None):
	""" Generates the dependencies for the XML files """
	abspath, targetFilename = os.path.split(node.get_abspath())
	
	stack = [targetFilename]
	dependencies = [targetFilename]
	
	while stack:
		nextFile = stack.pop()
		files = find_includes(os.path.join(abspath, nextFile))
		for file in files:
			if file not in dependencies:
				stack.append(file)
		dependencies.extend(files)
	
	dependencies.remove(targetFilename)
	return dependencies

# -----------------------------------------------------------------------------
def packet_emitter(target, source, env):
	try:
		path = env['path']
	except KeyError:
		path = '.'
	
	target = [os.path.join(path, "packets.cpp"),
			  os.path.join(path, "packets.hpp")]
	
	return (target, source)

def identifier_emitter(target, source, env):
	try:
		path = env['path']
	except KeyError:
		path = '.'
	
	target = [os.path.join(path, "identifier.hpp")]
	
	return (target, source)

def postman_emitter(target, source, env):
	try:
		path = env['path']
	except KeyError:
		path = '.'
	
	target = [os.path.join(path, "postman.cpp"),
			  os.path.join(path, "postman.hpp")]
	
	return (target, source)

def communication_emitter(target, source, env):
	try:
		path = env['path']
	except KeyError:
		path = '.'
	
	target = [os.path.join(path, "communication.hpp")]
	
	return (target, source)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.SetDefault(XPCC_SYSTEM_DESIGN_SCANNERS = {})
	env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'] = SCons.Script.Scanner(
					function = xml_include_scanner,
					skeys = ['.xml'])
	env['BUILDERS']['SystemCppPackets'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python "${XPCC_SYSTEM_BUILDER}/cpp_packets.py" ' \
					'--source_path ${TARGETS[0].dir} ' \
					'--header_path ${TARGETS[1].dir} ' \
					'$SOURCE',
				cmdstr="$SYSTEM_CPP_PACKETS_COMSTR"),
			emitter = packet_emitter,
			source_scanner = env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")
	
	env['BUILDERS']['SystemCppIdentifier'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python "${XPCC_SYSTEM_BUILDER}/cpp_identifier.py" ' \
					'--outpath ${TARGET.dir} ' \
					'$SOURCE',
				cmdstr="$SYSTEM_CPP_IDENTIFIER_COMSTR"),
			emitter = identifier_emitter,
			source_scanner = env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")
	
	env['BUILDERS']['SystemCppPostman'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python "${XPCC_SYSTEM_BUILDER}/cpp_postman.py" ' \
					'--container "${container}" ' \
					'--outpath ${TARGET.dir} ' \
					'$SOURCE',
				cmdstr="$SYSTEM_CPP_POSTMAN_COMSTR"),
			emitter = postman_emitter,
			source_scanner = env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")
	
	env['BUILDERS']['SystemCppCommunication'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python "${XPCC_SYSTEM_BUILDER}/cpp_communication.py" ' \
					'--outpath ${TARGET.dir} ' \
					'$SOURCE',
				cmdstr="$SYSTEM_CPP_COMMUNICATION_COMSTR"),
			emitter = communication_emitter,
			source_scanner = env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")
	
	if SCons.Script.ARGUMENTS.get('verbose') != '1':
		env['SYSTEM_CPP_PACKETS_COMSTR'] = "Generate packets from: $SOURCE"
		env['SYSTEM_CPP_IDENTIFIER_COMSTR'] = "Generate identifier from: $SOURCE"
		env['SYSTEM_CPP_POSTMAN_COMSTR'] = "Generate postman from: $SOURCE"
		env['SYSTEM_CPP_COMMUNICATION_COMSTR'] = "Generate communication stubs from: $SOURCE"

def exists(env):
	return True

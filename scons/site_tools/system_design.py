#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2010, 2014, Georgi Grinshpun
# Copyright (c) 2012, 2014, Sascha Schade
# Copyright (c) 2012, 2015, Niklas Hauser
# Copyright (c) 2014, Kevin Laeufer
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import re
import SCons
import SCons.Errors

# TODO make this more robust against whitespace etc.
includeExpression = re.compile(r'<include>(\S+)</include>', re.M)

# -----------------------------------------------------------------------------
def find_includes(env, file, include_path):
	""" Find include directives in an XML file """
	files = []
	line_count = 0
	for line in open(file).readlines():
		line_count = line_count + 1
		match = includeExpression.search(line)
		if match:
			filename = match.group(1)
			relative_to_file = os.path.join(os.path.dirname(os.path.abspath(file)), filename)
			relative_to_include_path = os.path.join(include_path, filename)
			# 1.) include file name can be absolut
			if os.path.isabs(filename):
				files.append(filename)
			# 2.) it could be a path relative to the files path
			#     this works just like #include "{filename}" in C/C++
			elif os.path.isfile(relative_to_file):
				files.append(relative_to_file)
			# 3.) it could be a path relative to the include path
			elif os.path.isfile(relative_to_include_path):
				files.append(relative_to_include_path)
			# 4.) Error!
			else:
				env.Error("Could not find include file '%s' in '%s:%s'" % (filename, file, line_count))
	return files

def xml_include_scanner(node, env, path, arg=None):
	""" Generates the dependencies for the XML files """
	abspath, targetFilename = os.path.split(node.get_abspath())

	stack = [targetFilename]
	dependencies = [targetFilename]

	while stack:
		nextFile = stack.pop()
		files = find_includes(env, os.path.join(abspath, nextFile), abspath)
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

def modm_task_caller_emitter(target, source, env):
	try:
		path = env['path']
	except KeyError:
		path = '.'
	
	target = [os.path.join(path, "caller.hpp")]
	return (target, source)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.SetDefault(MODM_SYSTEM_DESIGN_SCANNERS = {})
	env['MODM_SYSTEM_DESIGN_SCANNERS']['XML'] = SCons.Script.Scanner(
					function = xml_include_scanner,
					skeys = ['.xml'])
	env['BUILDERS']['SystemCppPackets'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python2 "${MODM_SYSTEM_BUILDER}/cpp_packets.py" ' \
					'--source_path ${TARGETS[0].dir} ' \
					'--header_path ${TARGETS[1].dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'$SOURCE',
				cmdstr="$SYSTEM_CPP_PACKETS_COMSTR"),
			emitter = packet_emitter,
			source_scanner = env['MODM_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")
	
	env['BUILDERS']['SystemCppIdentifier'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python2 "${MODM_SYSTEM_BUILDER}/cpp_identifier.py" ' \
					'--outpath ${TARGET.dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'$SOURCE',
				cmdstr="$SYSTEM_CPP_IDENTIFIER_COMSTR"),
			emitter = identifier_emitter,
			source_scanner = env['MODM_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")
	
	env['BUILDERS']['SystemCppPostman'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python2 "${MODM_SYSTEM_BUILDER}/cpp_postman.py" ' \
					'--container "${container}" ' \
					'--outpath ${TARGET.dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'$SOURCE',
				cmdstr="$SYSTEM_CPP_POSTMAN_COMSTR"),
			emitter = postman_emitter,
			source_scanner = env['MODM_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")
	
	env['BUILDERS']['SystemCppCommunication'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python2 "${MODM_SYSTEM_BUILDER}/cpp_communication.py" ' \
					'--outpath ${TARGET.dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'$SOURCE',
				cmdstr="$SYSTEM_CPP_COMMUNICATION_COMSTR"),
			emitter = communication_emitter,
			source_scanner = env['MODM_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")
	
	env['BUILDERS']['SystemCppXpccTaskCaller'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python2 "${MODM_SYSTEM_BUILDER}/cpp_modm_task_caller.py" ' \
					'--outpath ${TARGET.dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'$SOURCE',
				cmdstr="$SYSTEM_CPP_MODM_TASK_CALLER_COMSTR"),
			emitter = modm_task_caller_emitter,
			source_scanner = env['MODM_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")
	
	if SCons.Script.ARGUMENTS.get('verbose') != '1':
		env['SYSTEM_CPP_PACKETS_COMSTR'] = "Generate packets from: $SOURCE"
		env['SYSTEM_CPP_IDENTIFIER_COMSTR'] = "Generate identifier from: $SOURCE"
		env['SYSTEM_CPP_POSTMAN_COMSTR'] = "Generate postman from: $SOURCE"
		env['SYSTEM_CPP_COMMUNICATION_COMSTR'] = "Generate communication stubs from: $SOURCE"
		env['SYSTEM_CPP_MODM_TASK_CALLER_COMSTR'] = "Generate modm task callers from: $SOURCE"

def exists(env):
	return True

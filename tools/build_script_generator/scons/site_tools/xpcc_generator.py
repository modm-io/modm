#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2010, 2014, Georgi Grinshpun
# Copyright (c) 2012, 2014, 2017, Sascha Schade
# Copyright (c) 2012, 2015, 2017-2018, Niklas Hauser
# Copyright (c) 2014, Kevin Läufer
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2017, Michael Thies
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import re
import sys
import SCons
import SCons.Errors

# -----------------------------------------------------------------------------
# FIXME: Copied directly from `xmlparser/parser.py:222:Parser.find_include_file
def find_include_file(filename, include_file, include_paths, line_count):
	# 1.) include file name can be absolute
	if os.path.isabs(filename):
		return filename
	# 2.) it could be a path relative to the file's path
	#     this works just like #include "{filename}" in C/C++
	relative_to_file = os.path.abspath(os.path.join(os.path.dirname(include_file), filename))
	if os.path.isfile(relative_to_file):
		return relative_to_file
	# 3.) it could be a path relative to the include path
	for path in include_paths:
		relative_to_include_path = os.path.abspath(os.path.join(path, filename))
		if os.path.isfile(relative_to_include_path):
			return relative_to_include_path
	# 4.) Error!
	raise SCons.Errors.UserError("Could not find include file '%s' in '%s:%s'" % (filename, include_file, line_count))

# -----------------------------------------------------------------------------
def find_includes(env, file, include_paths):
	""" Find include directives in an XML file """
	includeExpression = re.compile(r'<include>(\S+)</include>', re.M)
	files = []
	for line_number, line in enumerate(open(file).readlines()):
		match = includeExpression.search(line)
		if match:
			files.append(find_include_file(match.group(1).strip(), file, include_paths, line_number))
	return files

global_include_paths = []
def xml_include_scanner(node, env, path, arg=None):
	""" Generates the dependencies for the XML files """
	global global_include_paths
	abspath, targetFilename = os.path.split(node.get_abspath())

	stack = [targetFilename]
	dependencies = [targetFilename]

	while stack:
		nextFile = stack.pop()
		files = find_includes(env, os.path.join(abspath, nextFile), [abspath] + global_include_paths)
		for file in files:
			if file not in dependencies:
				stack.append(file)
		dependencies.extend(files)

	dependencies.remove(targetFilename)
	return dependencies

# -----------------------------------------------------------------------------
def packet_emitter(target, source, env):
	path = env.get('path', '.')
	target = [os.path.join(path, "packets.cpp"),
			  os.path.join(path, "packets.hpp")]

	return (target, source)

def identifier_emitter(target, source, env):
	path = env.get('path', '.')
	target = [os.path.join(path, "identifier.hpp")]

	return (target, source)

def postman_emitter(target, source, env):
	path = env.get('path', '.')
	target = [os.path.join(path, "postman.cpp"),
			  os.path.join(path, "postman.hpp")]
	return (target, source)

def communication_emitter(target, source, env):
	path = env.get('path', '.')
	target = [os.path.join(path, "communication.hpp")]
	return (target, source)

def xpcc_task_caller_emitter(target, source, env):
	path = env.get('path', '.')
	target = [os.path.join(path, "caller.hpp")]
	return (target, source)

def xpcc_communication_header(env, xmlfile, container, path=None, dtdPath=None, namespace=None, include_paths=None):
	global global_include_paths
	if path is None:
		path = '.';
	if dtdPath is None:
		dtdPath = os.path.join(env['XPCC_SYSTEM_DESIGN'], "xml/dtd")
	if namespace is None:
		namespace = 'robot'
	if include_paths is None:
		include_paths = [os.path.abspath('.')]
	else:
		include_paths = [os.path.abspath(p) for p in include_paths]
	global_include_paths = include_paths
	include_paths = " ".join(['--include_path "{}"'.format(p) for p in include_paths])

	files  = env.SystemCppPackets(xmlfile, path=path, dtdPath=dtdPath, namespace=namespace, include_paths=include_paths)
	files += env.SystemCppIdentifier(xmlfile, path=path, dtdPath=dtdPath, namespace=namespace, include_paths=include_paths)
	files += env.SystemCppCommunication(xmlfile, path=path, dtdPath=dtdPath, namespace=namespace, include_paths=include_paths)
	files += env.SystemCppXpccTaskCaller(xmlfile, path=path, dtdPath=dtdPath, namespace=namespace, include_paths=include_paths)
	files += env.SystemCppPostman(source=xmlfile, path=path, dtdPath=dtdPath, namespace=namespace, include_paths=include_paths,
	                              target='postman', container=container)

	source = []
	for file in files:
		if file.name.endswith('.cpp'):
			source.append(file)

	return source

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.SetDefault(XPCC_SYSTEM_DESIGN_SCANNERS = {})
	env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'] = SCons.Script.Scanner(
					function = xml_include_scanner,
					skeys = ['.xml'])
	env['BUILDERS']['SystemCppPackets'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python3 "${XPCC_SYSTEM_DESIGN}/builder/cpp_packets.py" ' \
					'--source_path ${TARGETS[0].dir} ' \
					'--header_path ${TARGETS[1].dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'${include_paths} ' \
					'$SOURCE',
				cmdstr="$XPCC_PACKETS_COMSTR"),
			emitter = packet_emitter,
			source_scanner = env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")

	env['BUILDERS']['SystemCppIdentifier'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python3 "${XPCC_SYSTEM_DESIGN}/builder/cpp_identifier.py" ' \
					'--outpath ${TARGET.dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'${include_paths} ' \
					'$SOURCE',
				cmdstr="$XPCC_IDENTIFIER_COMSTR"),
			emitter = identifier_emitter,
			source_scanner = env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")

	env['BUILDERS']['SystemCppPostman'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python3 "${XPCC_SYSTEM_DESIGN}/builder/cpp_postman.py" ' \
					'--container "${container}" ' \
					'--outpath ${TARGET.dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'${include_paths} ' \
					'$SOURCE',
				cmdstr="$XPCC_POSTMAN_COMSTR"),
			emitter = postman_emitter,
			source_scanner = env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")

	env['BUILDERS']['SystemCppCommunication'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python3 "${XPCC_SYSTEM_DESIGN}/builder/cpp_communication.py" ' \
					'--outpath ${TARGET.dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'${include_paths} ' \
					'$SOURCE',
				cmdstr="$XPCC_COMM_STUBS_COMSTR"),
			emitter = communication_emitter,
			source_scanner = env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")

	env['BUILDERS']['SystemCppXpccTaskCaller'] = \
		SCons.Script.Builder(
			action = SCons.Action.Action(
				'python3 "${XPCC_SYSTEM_DESIGN}/builder/cpp_xpcc_task_caller.py" ' \
					'--outpath ${TARGET.dir} ' \
					'--dtdpath "${dtdPath}" ' \
					'--namespace "${namespace}" ' \
					'${include_paths} ' \
					'$SOURCE',
				cmdstr="$XPCC_TASK_CALLER_COMSTR"),
			emitter = xpcc_task_caller_emitter,
			source_scanner = env['XPCC_SYSTEM_DESIGN_SCANNERS']['XML'],
			single_source = True,
			target_factory = env.fs.Entry,
			src_suffix = ".xml")

	env.AddMethod(xpcc_communication_header, 'XpccCommunication')

def exists(env):
	return True

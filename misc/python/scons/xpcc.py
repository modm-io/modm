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
import re
import string
import utils.files
import xpcc_configparser

from SCons.Script import *

# -----------------------------------------------------------------------------
def buildpath(env, path, strip_extension=False):
	""" Relocate path from source directory to build directory
	"""
	if strip_extension:
		path = os.path.splitext(path)[0]
	
	if os.path.isabs(path) or path.startswith('..'):
		# if the file is not in a subpath of the current directory
		# build it in the root directory of the build path
		path = os.path.basename(path)
	
	return os.path.join(env['XPCC_BUILDPATH'], path)

def library_buildpath(env, path, strip_extension=False):
	if strip_extension:
		path = os.path.splitext(path)[0]
	
	result = os.path.join(env['XPCC_ROOTPATH'], 'build', 'library',
						  env['XPCC_LIBRARY_NAME'], path)
	
	return os.path.abspath(result)

def find_files(env, path, unittest=None, ignore=None):
	scanner = utils.files.Scanner(env, unittest)
	scanner.scan(path=path, ignore=ignore)
	return scanner

def require_architecture(env, architecture):
	if re.match(architecture, env['ARCHITECTURE']):
		return True
	else:
		return False

def check_defines(env):
	# FIXME
	#env.Alias('config', env.ShowConfiguration())
	
	projectConfig = env['XPCC_CONFIG']['defines'].keys()
	keys = env['XPCC_LIBRARY_DEFINES'].keys()
	keys.sort()
	
	print ""
	print "Defines:"
	for key in keys:
		default = env['XPCC_LIBRARY_DEFINES'][key]
		if key in projectConfig:
			value = env['XPCC_CONFIG']['defines'][key]
			projectConfig.remove(key)
			if value != default:
				print "  %s => %s (default: %s)" % (key.upper(), value, default)
				continue
		
		print "  %s => %s" % (key.upper(), default)
	
	if projectConfig:
		print "\nUnused Defines:"
		for key in projectConfig:
			print "  %s => %s" % (key.upper(), env['XPCC_CONFIG']['defines'][key])
	print ""

def xpcc_library(env):
	# set new buildpath for the library, path musst be relativ to the
	# SConscript directory!
	env.Append(CPPPATH = [os.path.join(env['XPCC_ROOTPATH'], 'src')])
	
	# build library
	library, defines = env.SConscript(
			os.path.join(env['XPCC_ROOTPATH'], 'src', 'SConscript'),
			exports = 'env')
	
	env['XPCC_LIBRARY_DEFINES'] = defines.copy()
	for key in defines.iterkeys():
		if key in env['XPCC_CONFIG']['defines']:
			env['XPCC_LIBRARY_DEFINES'][key] = env['XPCC_CONFIG']['defines'][key]
	
	# generate 'xpcc_config.hpp'
	substitutions = {
		'defines': '\n'.join(["#define %s %s" % (key.upper(), value) \
				for key, value in env['XPCC_LIBRARY_DEFINES'].iteritems()]),
		'name': env['XPCC_CONFIG']['general']['name']
	}
	file = env.SimpleTemplate(
			target = env.LibraryBuildpath('xpcc_config.hpp'),
			source = os.path.join(env['XPCC_ROOTPATH'], 
								  'misc/templates/xpcc_config.hpp.in'),
			SUBSTITUTIONS = substitutions)
	
	env.Append(LIBS = ['robot'])
	env.Append(LIBPATH = [env.LibraryBuildpath('.')])
	
	return library

def xpcc_header(env, xmlfile):
	env.Append(CPPPATH = [os.path.join(os.path.dirname(xmlfile), "..")])
	
	files  = env.SystemCppPackets(xmlfile)
	files += env.SystemCppIdentifier(xmlfile)
	if 'communication' in env['XPCC_CONFIG']:
		files += env.SystemCppPostman(target='postman.hpp', source=xmlfile, container=env['XPCC_CONFIG']['communication']['container'])
	
	source = []
	for file in files:
		if file.name.endswith('.cpp'):
			source.append(file)
	
	return source

def generate_defines(env, filename='defines.hpp'):
	defines = env['XPCC_CONFIG']['defines']
	if defines:
		substitutions = {
			'defines': '\n'.join(["#define %s %s" % (key.upper(), value) \
					for key, value in defines.iteritems()])
		}
		file = env.SimpleTemplate(
				target = filename,
				source = os.path.join(env['XPCC_ROOTPATH'], 
									  'misc/templates/defines.hpp.in'),
				SUBSTITUTIONS = substitutions)
		return file
	else:
		return None

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(buildpath, 'Buildpath')
	env.AddMethod(library_buildpath, 'LibraryBuildpath')
	env.AddMethod(find_files, 'FindFiles')
	env.AddMethod(require_architecture, 'RequireArchitecture')
	env.AddMethod(check_defines, 'ShowConfiguration')
	env.AddMethod(xpcc_library, 'XpccLibrary')
	#env.AddMethod(xpcc_header, 'XpccCommunicationHeader')
	env.AddMethod(xpcc_header, 'XpccGenerics')
	env.AddMethod(generate_defines, 'Defines')

def exists(env):
	return True

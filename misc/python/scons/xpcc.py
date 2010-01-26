#!/usr/bin/env python
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
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
import ConfigParser

from SCons.Script import *

# -----------------------------------------------------------------------------
class FileScanner:
	
	def __init__(self, env):
		self.env = env
	
	def scan(self, path):
		""" Scan a directory for source files
		
		Provides the following attributes to collect the results:
		sources		- list of source files
		header		- list of header files
		defines		- dictionary with defines needed by the source files
		"""
		parser = ConfigParser.RawConfigParser()
		
		self.sources = []
		self.header = []
		self.defines = {}
		
		basepath = path
		for path, directories, files in os.walk(basepath):
			# exclude the SVN-directories
			if '.svn' in directories:
				directories.remove('.svn')
			
			if 'build.cfg' in files:
				parser.read(os.path.join(path, 'build.cfg'))
				
				try:
					target = parser.get('build', 'target')
					if not re.match(target, self.env['ARCHITECTURE']):
						# if the this directory should be excluded, remove all the
						# subdirectories from the list to exclude them as well
						tempDirectories = directories[:]
						for d in tempDirectories:
							directories.remove(d)
						continue
				except ConfigParser.NoSectionError:
					pass
				
				try:
					for item in parser.items('defines'):
						self.defines[item[0]] = item[1]
				except ConfigParser.NoSectionError:
					pass
			
			for file in files:
				extension = os.path.splitext(file)[1]
				filename = os.path.relpath(os.path.join(path, file))
				
				if extension in ['.cpp', '.c', '.S']:
					self.sources.append(filename)
				elif extension in ['.h', '.hpp']:
					self.header.append(filename)

# -----------------------------------------------------------------------------
def generate_environment(env, config, buildpath = None, rootpath = None):
	try:
		parser = ConfigParser.RawConfigParser()
		parser.read(config)
	except TypeError:
		print "You need to specify a configuration file via 'scons config=path/to/file'!"
		Exit(1)
	
	# Create a build environment
	architecture = parser.get('target', 'architecture')
	if architecture == 'atmega' or architecture == 'atxmega':
		device = parser.get('target', 'device')
		clock = parser.get('target', 'clock')
		new = Environment(
				ARCHITECTURE = architecture + '/' + device,
				AVR_DEVICE = device,
				AVR_CLOCK = clock,
				tools = ['avr', 'doxygen', 'template', 'unittest', 'xpcc', 'utils'],
				toolpath = env['toolpath'],
				LIBS = [''],
				LIBPATH = [])
	elif architecture == 'pc':
		device = 'pc'
		clock = ''
		new = Environment(
				ARCHITECTURE = architecture,
				tools = ['pc', 'doxygen', 'template', 'unittest', 'xpcc', 'utils'],
				toolpath = env['toolpath'],
				LIBS = ['boost_thread-mt'],
				LIBPATH = ['/usr/lib/'])
	else:
		print "Unknown architecture '%s'!" % architecture
		Exit(1)
	
	# extract the rootpath from the toolpath
	xpcc_rootpath = os.path.normpath(env['toolpath'][0])
	xpcc_rootpath = os.path.normpath(xpcc_rootpath[:-len('misc/python/scons')])
	
	project_name = parser.get('general', 'name')
	
	if buildpath is None:
		try:
			buildpath = parser.get('general', 'buildpath')
		except ConfigParser.NoOptionError:
			if rootpath is None:
				print "Don't know where to build the files! Please specify either 'rootpath' or 'buildpath'"
				Exit(1)
			
			buildpath = os.path.join(rootpath, 'build/$name')
	
	try:
		xpcc_library_name = parser.get('general', 'library_name')
	except ConfigParser.NoOptionError:
		xpcc_library_name = project_name
	
	path_substitutions = {
		'name': project_name,
		'library_name': xpcc_library_name,
		'arch': architecture,
		'device': device
	}
	buildpath = string.Template(buildpath).safe_substitute(path_substitutions)
	
	new['XPCC_LIBRARY_NAME'] = xpcc_library_name
	new['XPCC_ROOTPATH'] = xpcc_rootpath
	new['XPCC_BUILDPATH'] = buildpath
	new['XPCC_CONFIG_FILE'] = os.path.abspath(config)
	new.Append(CPPPATH = [buildpath, '.'])
	
	cnf = {}
	for section in parser.sections():
		for option in parser.options(section):
			key = '%s.%s' % (section, option)
			cnf[key] = parser.get(section, option)
	new['XPCC_CONFIG'] = cnf
	
	try:
		# create 'defines.h'
		substitutions = {
			'defines': '\n'.join(["#define %s %s" % (item[0].upper(), item[1]) for item in parser.items('defines')]),
			'name': project_name
		}
		new.SimpleTemplate(target = new.Buildpath('defines.h'),
					source = os.path.join(xpcc_rootpath, 'misc/templates/defines.h.in'),
					SUBSTITUTIONS = substitutions)
	except ConfigParser.NoSectionError:
		pass
	
	return new

def xpcc_library(env):
	include_path = os.path.join(env['XPCC_ROOTPATH'], 'src')
	buildpath = os.path.join(env['XPCC_ROOTPATH'], 'build/library/%s' % env['XPCC_LIBRARY_NAME'])
	
	env.Append(CPPPATH = [include_path])
	env.Append(LIBS = ['robot'])
	env.Append(LIBPATH = [buildpath])
	
	file = os.path.join(buildpath, 'librobot.a')
	action = "@scons -C %s -Q config=%s" % (include_path, env['XPCC_CONFIG_FILE'])
	if ARGUMENTS.get('verbose') == '1':
		action += ' verbose=1'
	library = env.Command(file, [], action)
	
	env.Depends(library, SCons.Node.Python.Value(env['XPCC_CONFIG']))
	env.AlwaysBuild(library)
	
	# remove the build directory when the library should be cleaned
	env.Clean(library, buildpath)
	# TODO remove this with a call of 'scons -c' perhaps via GetOption("clean") 
	
	return library

def find_files(env, path):
	scanner = FileScanner(env)
	scanner.scan(path)
	return scanner

def buildpath(env, path, strip_extension=False):
	""" Relocate path from source directory to build directory
	"""
	if strip_extension:
		path = os.path.splitext(path)[0]
	
	path = os.path.relpath(path)
	if os.path.isabs(path) or path.startswith('..'):
		# if the file is not in a subpath of the current directory
		# build it in the root directory of the build path
		path = os.path.basename(path)
	
	return os.path.join(env['XPCC_BUILDPATH'], path)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(generate_environment, 'GenerateEnvironment')
	env.AddMethod(xpcc_library, 'XpccLibrary')
	env.AddMethod(find_files, 'FindFiles')
	env.AddMethod(buildpath, 'Buildpath')

def exists(env):
	return True

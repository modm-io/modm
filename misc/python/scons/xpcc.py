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
import ConfigParser

from SCons.Script import *

# -----------------------------------------------------------------------------
class ParserException(Exception):
	pass

class MyConfigParser(ConfigParser.RawConfigParser):
	
	def read(self, filename):
		return ConfigParser.RawConfigParser.read(self, filename)
	
	def get(self, section, option, default=None):
		try:
			return ConfigParser.RawConfigParser.get(self, section, option)
		except ConfigParser.NoOptionError, e:
			if default is not None:
				return default
			else:
				raise ParserException(e)
	
	def items(self, section):
		try:
			return ConfigParser.RawConfigParser.items(self, section)
		except ConfigParser.NoSectionError, e:
			raise ParserException(e)

# -----------------------------------------------------------------------------
class FileScanner:
	
	def __init__(self, env, unittest=None):
		""" Constructor
		
		Keyword arguments:
		env		 -	SCons environment
		unittest -	This variable has three states:
					None => select all files
					False => exclude files from subfolders named 'test'
					True => select only files in folders named 'test'
		"""
		self.env = env
		self.unittest = unittest
	
	def scan(self, path):
		""" Scan a directory for source files
		
		Provides the following attributes to collect the results:
		sources		- list of source files
		header		- list of header files
		defines		- dictionary with defines needed by the source files
		"""
		parser = MyConfigParser()
		
		self.sources = []
		self.header = []
		self.testHeader = []
		self.testSources = []
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
				except ParserException:
					pass
			
			for file in files:
				extension = os.path.splitext(file)[1]
				filename = os.path.join(path, file)
				
				if extension in ['.cpp', '.c', '.S']:
					# append source files
					if os.path.normpath(path).endswith(os.sep + 'test'):
						if self.unittest is True:
							self.testSources.append(filename)
						else:
							continue
					else:
						if self.unittest is True:
							continue
					self.sources.append(filename)
				
				elif extension in ['.h', '.hpp']:
					# append header files
					if os.path.normpath(path).endswith(os.sep + 'test'):
						if self.unittest is True:
							self.testHeader.append(filename)
						else:
							continue
					else:
						if self.unittest is True:
							continue
					self.header.append(filename)

# -----------------------------------------------------------------------------
def generate_environment(env, configfile, rootpath, buildpath = None):
	""" Creates a new build environment
	
	Keyword arguments:
	configfile	-	Path to the configuration file
	rootpath	-	Path to the xpcc/trunk directory
	"""
	try:
		parser = MyConfigParser()
		parser.read(configfile)
		
		# read configuration
		architecture = parser.get('build', 'architecture')
		if architecture == 'pc':
			device = 'pc'
			clock = ''
		else:
			device = parser.get('build', 'device')
			clock = parser.get('build', 'clock')
		
		project_name = parser.get('general', 'name')
		library_name = parser.get('build', 'library_name', project_name)
		
		if buildpath is None:
			buildpath = parser.get('build', 'buildpath', os.path.join(rootpath, 'build/$name'))
		
		rootpath = os.path.abspath(rootpath)
		buildpath = os.path.abspath(buildpath)
	except ParserException, msg:
		print msg
		Exit(1)
	
	configuration = { 'defines': {} }
	for section in parser.sections():
		s = {}
		for option in parser.options(section):
			s[option] = parser.get(section, option)
		configuration[section] = s
	
	# Create the build environment
	if architecture == 'atmega' or architecture == 'atxmega':
		new = Environment(
				ARCHITECTURE = architecture + '/' + device,
				AVR_DEVICE = device,
				AVR_CLOCK = clock,
				tools = ['avr', 'doxygen', 'template', 'unittest', 'xpcc', 'utils'],
				toolpath = env['toolpath'],
				LIBS = [''],
				LIBPATH = [])
	elif architecture == 'pc':
		new = Environment(
				ARCHITECTURE = architecture + '/' + device,
				tools = ['pc', 'doxygen', 'template', 'unittest', 'xpcc', 'utils'],
				toolpath = env['toolpath'],
				LIBS = ['boost_thread-mt'],
				LIBPATH = ['/usr/lib/'])
	else:
		print "Unknown architecture '%s'!" % architecture
		Exit(1)
	
	path_substitutions = {
		'arch': architecture,
		'device': device,
		'name': project_name,
		'library_name': library_name,
	}
	buildpath = string.Template(buildpath).safe_substitute(path_substitutions)
	
	new['XPCC_LIBRARY_NAME'] = library_name
	new['XPCC_ROOTPATH'] = rootpath
	new['XPCC_BUILDPATH'] = buildpath
	new['XPCC_CONFIG'] = configuration
	new['XPCC_CONFIG_FILE'] = os.path.abspath(configfile)
	
	return new

def xpcc_library(env):
	# set new buildpath for the library, path musst be relativ to the
	# SConscript directory!
	env.Append(CPPPATH = [os.path.join(env['XPCC_ROOTPATH'], 'src')])
	
	# build library
	library, defines = env.SConscript(
			os.path.join(env['XPCC_ROOTPATH'], 'src', 'SConscript'),
			exports = 'env')
	
	# generate 'defines.h'
	substitutions = {
		'defines': '\n'.join(["#define %s %s" % (key.upper(), value) for key, value in env['XPCC_CONFIG']['defines'].iteritems()]),
		'name': env['XPCC_CONFIG']['general']['name']
	}
	file = env.SimpleTemplate(
			target = env.LibraryBuildpath('defines.h'),
			source = os.path.join(env['XPCC_ROOTPATH'], 'misc/templates/defines.h.in'),
			SUBSTITUTIONS = substitutions)
	
	env.Append(LIBS = ['robot'])
	env.Append(LIBPATH = [env.LibraryBuildpath('.')])
	
	return (library, defines)

def find_files(env, path, unittest=None):
	scanner = FileScanner(env, unittest)
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

def library_buildpath(env, path, strip_extension=False):
	if strip_extension:
		path = os.path.splitext(path)[0]
	
	result = os.path.join(env['XPCC_ROOTPATH'], 'build', 'library',
						  env['XPCC_LIBRARY_NAME'], path)
	
	return os.path.abspath(result)


def require_architecture(env, architecture):
	if re.match(architecture, env['ARCHITECTURE']):
		return True
	else:
		return False

def check_defines(env, defines):
	# TODO check which defines are missing and which are unused
	print "not implemented yet"

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(generate_environment, 'GenerateEnvironment')
	env.AddMethod(xpcc_library, 'XpccLibrary')
	env.AddMethod(find_files, 'FindFiles')
	env.AddMethod(buildpath, 'Buildpath')
	env.AddMethod(library_buildpath, 'LibraryBuildpath')
	env.AddMethod(require_architecture, 'RequireArchitecture')
	env.AddMethod(check_defines, 'CheckDefines')

def exists(env):
	return True

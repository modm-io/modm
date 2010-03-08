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
import xpcc_configparser as configparser

from SCons.Script import *

def listify(node):
	return [node,] if (not isinstance(node, list)) else node

# -----------------------------------------------------------------------------
class FileScanner:
	
	HEADER = ['.h', '.hpp']
	SOURCE = ['.cpp', '.c', '.S']
	
	def __init__(self, env, unittest=None):
		""" Constructor
		
		Keyword arguments:
		env		 -	SCons environment
		unittest -	This variable has three states:
					None  => select all files
					False => exclude files from subfolders named 'test'
					True  => select only files in folders named 'test'
		"""
		self.env = env
		self.unittest = unittest
	
	def scan(self, path):
		""" Scan directories for source files
		
		Provides the following attributes to collect the results:
		sources		- list of source files
		header		- list of header files
		defines		- dictionary with defines needed by the source files
		"""
		parser = configparser.XpccConfigParser()
		
		self.sources = []
		self.header = []
		self.testHeader = []
		self.testSources = []
		self.defines = {}
		
		pathlist = listify(path)
		
		for basepath in pathlist:
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
					except configparser.ParserException:
						pass
					
					try:
						for item in parser.items('defines'):
							self.defines[item[0]] = item[1]
					except configparser.ParserException:
						pass
				
				for file in files:
					extension = os.path.splitext(file)[1]
					filename = os.path.join(path, file)
					
					if extension in self.SOURCE:
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
					
					elif extension in self.HEADER:
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
		
	def append(self, files):
		for file in listify(files):
			filename = str(file)
			extension = os.path.splitext(filename)[1]
			
			if extension in self.SOURCE:
				self.sources.append(filename)
			elif extension in self.HEADER:
				self.header.append(filename)


# -----------------------------------------------------------------------------
def buildpath(env, path, strip_extension=False):
	""" Relocate path from source directory to build directory
	"""
	if strip_extension:
		path = os.path.splitext(path)[0]
	
	#path = os.path.relpath(path)
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

def find_files(env, path, unittest=None):
	scanner = FileScanner(env, unittest)
	scanner.scan(path)
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
	defines.update(env['XPCC_CONFIG']['defines'])
	
	# generate 'config.h'
	substitutions = {
		'defines': '\n'.join(["#define %s %s" % (key.upper(), value) \
				for key, value in defines.iteritems()]),
		'name': env['XPCC_CONFIG']['general']['name']
	}
	file = env.SimpleTemplate(
			target = env.LibraryBuildpath('config.h'),
			source = os.path.join(env['XPCC_ROOTPATH'], 
								  'misc/templates/config.h.in'),
			SUBSTITUTIONS = substitutions)
	
	env.Append(LIBS = ['robot'])
	env.Append(LIBPATH = [env.LibraryBuildpath('.')])
	
	return library

def xpcc_generics(env, xmlfile):
	env.Append(CPPPATH = [os.path.dirname(xmlfile)])
	
	source  = env.SystemCppPackets(xmlfile)
	source += env.SystemCppIdentifier(xmlfile)
	
	return source

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(buildpath, 'Buildpath')
	env.AddMethod(library_buildpath, 'LibraryBuildpath')
	env.AddMethod(find_files, 'FindFiles')
	env.AddMethod(require_architecture, 'RequireArchitecture')
	env.AddMethod(check_defines, 'ShowConfiguration')
	env.AddMethod(xpcc_library, 'XpccLibrary')
	env.AddMethod(xpcc_generics, 'XpccGenerics')

def exists(env):
	return True

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
				filename = os.path.join(path, file)
				
				if extension in ['.cpp', '.c', '.S']:
					self.sources.append(filename)
				elif extension in ['.h', '.hpp']:
					self.header.append(filename)

# -----------------------------------------------------------------------------
class EnviromentBuilder:
	
	def __init__(self, enviroment, config_file):
		try:
			self.config = ConfigParser.RawConfigParser()
			self.config.read(config_file)
		except TypeError:
			print "You need to specify a configuration file via 'scons config=path/to/file'!"
			Exit(1)
		
		self.config_file = config_file
		self.project_name = self.config.get('general', 'name')
		self.toolpath = enviroment['toolpath']
		
		# extract the rootpath from the toolpath
		self.rootpath = os.path.normpath(self.toolpath[0])
		self.rootpath = os.path.normpath(self.rootpath[:-len('misc/python/scons')])
		
		# Create a build environment
		self.architecture = self.config.get('target', 'architecture')
		
		self.defines = {}
		for item in self.config.items('defines'):
			self.defines[item[0]] = item[1]
	
	def generate_build_environment(self, library=False):
		if library:
			self._buildpath = os.path.join(self.rootpath, 'build/library/%s' % self.project_name)
		else:
			self._buildpath = os.path.join(self.rootpath, 'build/%s' % self.project_name)
		
		if self.architecture == 'atmega' or self.architecture == 'atxmega':
			self.device = self.config.get('target', 'device')
			self.clock = self.config.get('target', 'clock')
			self.env = Environment(
					ARCHITECTURE = architecture + '/' + device,
					AVR_DEVICE = self.device,
					AVR_CLOCK = self.clock,
					tools = ['avr', 'doxygen', 'unittest', 'xpcc'],
					toolpath = self.toolpath,
					LIBS=[''],
					LIBPATH=[],
					CPPPATH=[self._buildpath, '.'])
		elif self.architecture == 'pc':
			self.env = Environment(
					ARCHITECTURE = self.architecture,
					tools = ['pc', 'doxygen', 'unittest', 'xpcc'],
					toolpath = self.toolpath,
					LIBS = ['boost_thread-mt'],
					LIBPATH = ['/usr/lib/'],
					CPPPATH = [self._buildpath, '.'])
		else:
			print "Unknown architecture '%s'!" % architecture
			Exit(1)
		
		self.env['XPCC_ROOTPATH'] = self.rootpath
		self.env['XPCC_PROJECT_NAME'] = self.project_name
		self.env['XPCC_CONFIG_FILE'] = os.path.abspath(self.config_file)
		
		self.env.GenerateConfig(target = self.buildpath('defines.h'),
								source = None,
								DEFINES = self.defines)
		
		return self.env
	
	def buildpath(self, path, strip_extension=False):
		""" Relocate path from source directory to the build directory
		"""
		if strip_extension:
			path = os.path.splitext(path)[0]
		
		return os.path.join(self._buildpath, path)

# -----------------------------------------------------------------------------
def generate_enviroment_builder(env, config_file):
	return EnviromentBuilder(env, config_file)

def xpcc_library(env):
	include_path = os.path.join(env['XPCC_ROOTPATH'], 'src')
	buildpath = os.path.join(env['XPCC_ROOTPATH'], 'build/library/%s' % env['XPCC_PROJECT_NAME'])
	
	env.Append(CPPPATH = [include_path])
	env.Append(LIBS = ['robot'])
	env.Append(LIBPATH = [buildpath])
	
	file = os.path.join(buildpath, 'librobot.a')
	action = "@scons -C %s config=%s" % (include_path, env['XPCC_CONFIG_FILE'])
	library = env.Command(file, [], action)
	
	# remove the build directory when the library should be cleaned
	Clean(library, buildpath)
	
	# TODO remove this with a call of 'scons -c' perhaps via GetOption("clean") 
	
	return library

def run_program(env, program):
	return env.Command('thisfileshouldnotexist', program, '@' + program[0].abspath)

def phony_target(env, **kw):
	for target, action in kw.items():
		env.AlwaysBuild(env.Alias(target, [], action))

def find_files(env, path):
	scanner = FileScanner(env)
	scanner.scan(path)
	return scanner

# -----------------------------------------------------------------------------
config_template = """#ifndef DEFINES_H
#define	DEFINES_H

%(defines)s

#endif
"""

def configuration_action(target, source, env):
	substitutions = {
		'defines': '\n'.join(["#define %s %s" % (key.upper(), value) for key, value in env['DEFINES'].iteritems()])
	}
	
	if not source:
		input = config_template
	else:
		input = open(source[0].abspath).read()
	
	# create file
	open(target[0].abspath, 'w').write(input % substitutions)
	return 0

def configuration_emitter(target, source, env):
	Depends(target, SCons.Node.Python.Value(env['DEFINES']))
	return target, source

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(generate_enviroment_builder, 'EnviromentBuilder')
	env.AddMethod(xpcc_library, 'xpccLibrary')
	env.AddMethod(run_program, 'Run')
	env.AddMethod(phony_target, 'Phony')
	env.AddMethod(find_files, 'FindFiles')
	
	env.Append(BUILDERS = {
		'GenerateConfig': Builder(
				action = SCons.Action.Action(configuration_action, "Generate: $TARGET"),
				suffix = '.h',
				emitter = configuration_emitter,
				target_factory = env.fs.File)
	})

def exists(env):
	return True

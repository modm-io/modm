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
# $Id: pc.py 107 2009-10-29 22:00:59Z dergraaf $

import os
import re
import ConfigParser

from SCons.Script import *

# -----------------------------------------------------------------------------
class FileScanner:
	
	def __init__(self, env):
		self.env = env
	
	def scan(self, path):
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
	
	def get_source_files(self):
		return self.sources
	
	def get_header_files(self):
		return self.header
	
	def get_defines(self):
		return self.defines
	
def create_file_scanner(env):
	return FileScanner(env)

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
	env.AddMethod(create_file_scanner, 'CreateFileScanner')
	
	env.Append(BUILDERS = {
		'GenerateConfig': Builder(
				action = SCons.Action.Action(configuration_action, "Generate: $TARGET"),
				suffix = '.h',
				emitter = configuration_emitter,
				target_factory = env.fs.File)
	})

def exists(env):
	return True

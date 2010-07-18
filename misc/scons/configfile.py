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
import ConfigParser

def listify(node):
	return [node,] if (not isinstance(node, list)) else node

# -----------------------------------------------------------------------------
class ParserException(Exception):
	pass

class Parser(ConfigParser.RawConfigParser):
	
	def read(self, filename):
		return ConfigParser.RawConfigParser.read(self, filename)
	
	def get(self, section, option, default=None):
		try:
			return ConfigParser.RawConfigParser.get(self, section, option)
		except (ConfigParser.NoOptionError,
				ConfigParser.NoSectionError), e:
			if default != None:
				return default
			else:
				raise ParserException(e)
	
	def getboolean(self, section, option, default=None):
		try:
			return ConfigParser.RawConfigParser.getboolean(self, section, option)
		except (ConfigParser.NoOptionError,
				ConfigParser.NoSectionError,
				ParserException), e:
			if default != None:
				return default
			else:
				raise ParserException(e)
	
	def items(self, section):
		try:
			return ConfigParser.RawConfigParser.items(self, section)
		except (ConfigParser.NoOptionError,
				ConfigParser.NoSectionError), e:
			raise ParserException(e)

# -----------------------------------------------------------------------------
class Scanner:
	
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
	
	def scan(self, path, ignore=None):
		""" Scan directories for source files
		
		Provides the following attributes to collect the results:
		sources		- list of source files
		header		- list of header files
		defines		- dictionary with defines needed by the source files
		"""
		parser = Parser()
		
		if ignore is None:
			ignoreList = []
		else:
			ignoreList = listify(ignore)
		pathlist = listify(path)
		
		self.sources = []
		self.header = []
		self.testHeader = []
		self.testSources = []
		self.defines = {}
		
		for basepath in pathlist:
			for path, directories, files in os.walk(basepath):
				# exclude the SVN-directories
				if '.svn' in directories:
					directories.remove('.svn')
				
				if 'build.cfg' in files:
					parser.read(os.path.join(path, 'build.cfg'))
					
					if self._excludeDirectory(parser):
						# if the this directory should be excluded, remove all the
						# subdirectories from the list to exclude them as well
						tempDirectories = directories[:]
						for d in tempDirectories:
							directories.remove(d)
						continue
					
					try:
						for item in parser.items('defines'):
							self.defines[item[0]] = item[1]
					except ParserException:
						pass
				
				for file in files:
					ignore = False
					for ignoreFile in ignoreList:
						if self._samefile(os.path.join(path, file), ignoreFile):
							ignore = True
							break
					if ignore:
						continue
					
					extension = os.path.splitext(file)[1]
					filename = os.path.join(path, file)
					path = os.path.normpath(path)
					
					if extension in self.SOURCE:
						# append source files
						if path.endswith(os.sep + 'test'):
							if self.unittest is True:
								self.testSources.append(filename)
							else:
								continue
						elif path.endswith(os.sep + 'examples'):
							continue	# TODO
						else:
							if self.unittest is True:
								continue
						self.sources.append(filename)
					
					elif extension in self.HEADER:
						# append header files
						if path.endswith(os.sep + 'test'):
							if self.unittest is True:
								self.testHeader.append(filename)
							else:
								continue
						elif path.endswith(os.sep + 'examples'):
							continue	# TODO
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
	
	def _excludeDirectory(self, parser):
		try:
			target = parser.get('build', 'target')
			if not re.match(target, self.env['ARCHITECTURE']):
				return True
		except ParserException:
			pass
		return False
	
	def _samefile(self, src, dst):
		# Macintosh, Unix
		if hasattr(os.path,'samefile'):
			try:
				return os.path.samefile(src, dst)
			except OSError:
				return False
		
		# All other platforms: check for same pathname.
		return (os.path.normcase(os.path.abspath(src)) ==
				os.path.normcase(os.path.abspath(dst)))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	def generate_configparser(env):
		return Parser()
	
	env.AddMethod(generate_configparser, 'ConfigParser')
	
	def find_files(env, path, unittest=None, ignore=None):
		scanner = Scanner(env, unittest)
		scanner.scan(path=path, ignore=ignore)
		return scanner
	
	env.AddMethod(find_files, 'FindFiles')

def exists(env):
	return True

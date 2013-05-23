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
import sys
import re
import ConfigParser

import SCons.Node
import SCons.Errors

def listify(node):
	return [node,] if (not isinstance(node, list) and \
					   not isinstance(node, SCons.Node.NodeList)) else node

# -----------------------------------------------------------------------------
class ParserException(Exception):
	pass

class Parser(ConfigParser.RawConfigParser):
	
	def read(self, filename):
		try:
			return ConfigParser.RawConfigParser.read(self, filename)
		except ConfigParser.ParsingError as e:
			raise SCons.Errors.UserError(str(e) + '\n')
	
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
class FileList(list):
	"""
	Special list for file object. Checks if a file is already in the list
	when adding it.
	"""
	def append(self, item):
		if hasattr(item, '__getitem__'):
			self.extend(item)
		else:
			self.__append(item)
	
	def __append(self, item):
		if not isinstance(item, SCons.Node.Node):
			item = SCons.Node.FS.default_fs.File(str(item))
			# print dir(item)
		if not self.__contains__(item):
			list.append(self, item)
	
	def extend(self, l):
		for item in l:
			self.append(item)
	
	def __iadd__(self, item):
		self.append(item)
		return self

# -----------------------------------------------------------------------------
class Scanner:
	
	HEADER = ['.h', '.hpp']
	SOURCE = ['.cpp', '.c', '.sx', '.S']
	
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
		if ignore is None:
			ignoreList = []
		else:
			ignoreList = listify(ignore)
		pathlist = listify(path)
		
		self.sources = FileList()
		self.header = FileList()
		self.defines = {}
		
		for basepath in pathlist:
			for path, directories, files in os.walk(basepath):
				if self._fileInList (path, ignoreList):
					directories = self._excludeDirectories(directories)
					continue
				
				if 'build.cfg' in files:
					parser = Parser()
					parser.read(os.path.join(path, 'build.cfg'))
					
					if self._directoryShouldBeExcluded(parser):
						directories = self._excludeDirectories(directories)
						continue
					
					try:
						for item in parser.items('defines'):
							self.defines[item[0]] = item[1]
					except ParserException:
						pass
				
				if self.unittest is None or not (self.unittest ^ path.endswith(os.sep + 'test')):
					# only check this directory for files if all directories
					# should be check or unittest is active and directory
					# ends with test.
					p = path + '/*'
					for source in self.SOURCE:
						files = self.env.Glob(p + source)
						
						for file in files[:]:
							for ignoreFile in ignoreList:
								if self._samefile(str(file), ignoreFile):
									files.remove(file)
						
						self.sources.extend(files)
						
					if self.unittest is True:
						for header in self.HEADER:
							self.header.extend(self.env.Glob(p + header))
	
	def __iadd__(self, item):
		self.append(item)
		return self	
		
	def append(self, files):
		for file in listify(files):
			filename = str(file)
			extension = os.path.splitext(filename)[1]
			if extension in self.SOURCE:
				self.sources.append(file)
			elif extension in self.HEADER:
				self.header.append(file)
	
	def _directoryShouldBeExcluded(self, parser):
		try:
			target = parser.get('build', 'target')
			
			if target.startswith('!'):
				target = target[1:]
				invert = True
			else:
				invert = False
			
			if not re.search(target, self.env['ARCHITECTURE']):
				return True ^ invert
			else:
				return False ^ invert
		except ParserException:
			return False
	
	def _excludeDirectories(self, directories):
		# if the this directory should be excluded, remove all the
		# subdirectories from the list to exclude them as well
		tempDirectories = directories[:]
		for d in tempDirectories:
			directories.remove(d)
		return directories
	
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
		
	def __str__(self):
		string = "Source files: "
		if self.sources:
			for s in self.sources:
				 string += "\n  " + str(s)
		else:
			string += "None"

		string += "\nHeader files: "
		if self.header:
			string += "\nHeader files:"
			for h in self.header:
				 string += "\n  " + str(h)	 
		else:
			string += "None"

		string += "\nDefines: "
		if self.defines:
	 		for d in self.defines:
				 string += "\n  " + str(d)
		else:
			string += "None"
		return string

	def _fileInList(self, file, fileList):
		# returns true if file (or directory) is contained in fileList
		for f in fileList:
			if self._samefile (file, f):
				return True
		return False

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

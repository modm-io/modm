#!/usr/bin/env python
#
# $Id$

import yaml
import os

class GlobalProperties:
	class AvrProperties:
		def __init__(self, properties):
			self.avrdude = properties.get('avrdude', {})
			self.device = properties['device']
			self.clock = properties['clock']
	
	def __init__(self, properties):
		self.target = properties.get('target', 'pc')
		
		if self.target == 'avr':
			self.avr = AvrProperties(properties.get('avr', {}))


class FileProperties:
	def __init__(self, filename):
		self.name = filename


class DirectoryProperties:
	def __init__(self, properties, globalProperties):
		
		self._enabled = True
		
		if properties:
			if properties.get('exclude', False):
				self._enabled = False
			if 'exclude-target' in properties and \
					properties['exclude-target'] == globalProperties.target:
				self._enabled = False
	
	def createFileProperties(self, filename):
		return FileProperties(filename)
	
	def shouldBeBuild(self):
		return self._enabled


class PropertyParser:
	
	PROPERTY_FILE = 'properties.yaml'
	
	def __init__(self, configFile, verbose=False):
		""" Create the parser.
		
		Keyword arguments:
		configFile		--	configuration file which specifies
							how to build the system.
		"""
		self.verbose = verbose
		try:
			properties = yaml.load(open(configFile))
		except yaml.YAMLError, e:
			print "Error in configuration file:", e
			Exit(1)
		except IOError, e:
			print "Could not open file '%s'!" % configFile
			Exit(1)
		
		self.globalProperties = GlobalProperties(properties)
	
	def parseDirectory(self, target):
		fileList = []
		for path, directories, files in os.walk(target):
			# exclude the SVN-directories
			if '.svn' in directories:
				directories.remove('.svn')
			
			build = self._parsePropertyFile(path)
			if build.shouldBeBuild():
				for file in files:
					extension = os.path.splitext(file)[1]
					if extension == '.cpp' or extension == '.c':
						filename = os.path.join(path[len(target):], file)
						fileList.append(build.createFileProperties(filename))
			else:
				# remove all the subdirectories from the list so they will
				# not be build
				for dir in directories:
					directories.remove(dir)
		
		return fileList
	
	def _parsePropertyFile(self, path):
		try:
			filename = os.path.join(path, self.PROPERTY_FILE)
			options = yaml.load(open(filename))
		except yaml.YAMLError, e:
			print "Error in configuration file:", e
			Exit(1)
		except IOError:
			options = None
		else:
			if self.verbose:
				print "load -", path
		
		return DirectoryProperties(options, self.globalProperties)
	
	def getGlobalProperties(self):
		return self.globalProperties

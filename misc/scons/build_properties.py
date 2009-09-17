#!/usr/bin/env python
#
# $Id$

import yaml
import os

def listify(object):
	""" Helper function to put a object in a list if the object
	isn't a list allready. """
	if isinstance(object, list):
		return object
	else:
		return [object]


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
		if properties:
			self._enabled = False
			for target in listify(properties['target']):
				if target == 'all' or target == globalProperties.target:
					self._enabled = True
		else:
			self._enabled = True
	
	def createFileProperties(self, filename):
		return FileProperties(filename)
	
	def shouldBeBuild(self):
		return self._enabled


class PropertyParser:
	""" Parser to read property-files and create a list of files to build. """
	PROPERTY_FILE = 'properties.yaml'
	
	def __init__(self, configFile, verbose=False):
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
		""" Reads recursively the property-files from the directories and
		returns a list of files to build.
		"""
		fileList = []
		for path, directories, files in os.walk(target):
			# exclude the SVN-directories
			if '.svn' in directories:
				directories.remove('.svn')
			
			build = self._parseDirectoryProperties(path)
			if build.shouldBeBuild():
				for file in files:
					extension = os.path.splitext(file)[1]
					if extension == '.cpp' or extension == '.c':
						filename = os.path.join(path[len(target):], file)
						fileList.append(build.createFileProperties(filename))
			else:
				# if the this directory should be excluded, remove all the
				# subdirectories from the list to exclude them as well
				for dir in directories:
					directories.remove(dir)
		
		return fileList
	
	def _parseDirectoryProperties(self, path):
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

#!/usr/bin/env python
#
# $Id$

import yaml
import os

class PropertyException(Exception):
	pass


def listify(object):
	"""
	Helper function to put a object in a list if the object 
	isn't a list already.
	"""
	if isinstance(object, list):
		return object
	else:
		return [object]


class GlobalProperties:
	"""
	Representation of the global property-file
	"""
	class AvrProperties:
		"""
		AVR specific properties.
		"""
		def __init__(self, properties):
			self.avrdude = properties.get('avrdude', {})
			self.device = properties['device']
			self.clock = properties['clock']
		
		def __str__(self):
			return "%s [%s]" % (self.device, self.clock)
	
	def __init__(self, properties, target=None):
		self.properties = properties
		
		if not target:
			target = self.properties.get('target', 'pc')
		self.target = target
		
		if self.target == 'atmega':
			self.avr = self.AvrProperties(self.properties.get('atmega', {}))
		elif self.target == 'atxmega':
			self.avr = self.AvrProperties(self.properties.get('atxmega', {}))
	
	def getLocalProperties(self, path, target, tag):
		"""
		Keywords arguments:
		path	--	
		target	--	
		tag		--	
		"""
		try:
			local = self.properties[tag]
			relpath = path[len(target):]
			for step in relpath.split(os.sep):
				local = local[step]
		except (KeyError, TypeError):
			local = None
		
		return local if (local) else {}


class FileProperties:
	""" Contains all information about a single source file.
	
	Attributes:
	 - name			filename relative to given directory
	 - basename		filename without path and extension
	 - abspath		absolute path to the file
	 - defines		dictionary containing all defines which should be set
					via commandline line when building the file
	"""
	def __init__(self, filename, abspath, defines=None):
		self.name = filename
		self.basename = os.path.splitext(os.path.basename(filename))[0]
		self.abspath = abspath
		if defines:
			self.defines = defines
		else:
			self.defines = {}
		self.defines['BASENAME'] = self.basename
	
	#def getDefines(self, globalDefines=None):
	#	"""
	#	Returns a dictionary with the defines for this file.
	#	"""
	#	if globalDefines:
	#		defines = self.defines.copy()
	#		defines.update(globalDefines)
	#		return defines
	#	else:
	#		return self.defines.copy()
	
	def update(self, env):
		"""
		Update the internal values with values from the external SCons
		build enviroment.
		
		Keyword arguments:
		env		--	SCons enviroment
		"""
		try:
			self.defines.update(env['CPPDEFINES'])
		except KeyError:
			pass
	
	def __cmp__(self, other):
		return cmp(self.name, other)


class ProjectProperties:
	"""
	Settings for a project
	
	Attributes:
	executable	--	name of the file. The executable should be named accordingly
	path		--	relative path to the root directory
	header		--	header files
	sources		--	source files
	libs		--	librarys
	includes	--	list of include paths for the compiler
	libpath		--	list of paths for the linker to look for librarys
	"""
	def __init__(self, path, tree):
		self.path = path
		
		self.header = []
		self.sources = []
		
		self.libs = []
		self.includes = []
		self.libpath = []
		
		if isinstance(tree, basestring):
			self.executable = tree
		else:
			self.executable = tree['executable']
			
			self.libs = self._updateList(self.libs, tree, 'libs')
			self.libpath = self._updateList(self.libpath, tree, 'libpath')
			self.includes = self._updateList(self.includes, tree, 'includes')
	
	def update(self, env):
		"""
		Update the internal values with values from the external SCons
		build enviroment.
		
		Keyword arguments:
		env		--	SCons enviroment
		"""
		for header in self.header:
			header.update(env)
		for source in self.sources:
			source.update(env)
		
		self.libs = self._updateList(self.libs, env, 'LIBS')
		self.includes = self._updateList(self.includes, env, 'CPPPATH')
		self.libpath = self._updateList(self.libpath, env, 'LIBPATH')
	
	def _updateList(self, list, dict, key):
		try:
			list += listify(dict[key])
		except KeyError:
			pass
		return list

class DirectoryProperties:
	""" Contains the information about a directory.
	
	Determines the type of the directory and what should be done with
	the files in it. Used to create a property description of single files.
	
	"""
	INACTIVE = 0
	UNKNOWN = 1
	ACTIVE = 2
	PROJECT = 3
	
	def __init__(self, path, tree, locals, globals):
		"""
		Constructor
		
		Keyword arguments:
		path	--	path to the directory
		tree	--	tree build form the properties.yaml file in the directory
		locals	--	current section of the property-tree corresponding to
					this directory
		globals	--	global property-tree
		
		"""
		self.path = path
		self.locals = locals
		self.tree = tree
		self.globals = globals
		self.status = self.INACTIVE
		
		if tree:
			# found a property-file => check what to do with this directory
			for target in listify(self.tree['target']):
				if target == 'all' or target == globals.target:
					self.status = self.ACTIVE
			
			if 'project' in self.tree and self.status == self.ACTIVE:
				self.status = self.PROJECT
		else:
			self.status = self.UNKNOWN
	
	def getFile(self, filename, abspath):
		defines  = self.locals.get('defines', {})
		return FileProperties(filename, abspath, defines)
	
	def getProject(self):
		return ProjectProperties(self.path, self.tree['project'])
	
	def isProject(self):
		return self.status == self.PROJECT
	
	def isEnabled(self):
		return (self.status != self.INACTIVE)
	
	def __str__(self):
		status = {	0: "inactive",
					1: "unknown",
					2: "active",
					3: "project" }[self.status]
		return "%s [%s]" % (self.path, status)


class PropertyParser:
	"""
	Parser to read property-files and create a list of files to build.
	"""
	sourcetypes = ['.cpp', '.c', '.S']
	headertypes = ['.h', '.hpp']
	
	def __init__(self, configFile, target=None, verbose=False):
		self.verbose = verbose
		try:
			properties = yaml.load(open(configFile))
		except yaml.YAMLError, e:
			raise PropertyException("Error in configuration file:", e)
		except IOError, e:
			raise PropertyException("Could not open file '%s'!" % configFile)
		
		self.globals = GlobalProperties(properties, target)
	
	def parseDirectory(self, target, tag):
		"""
		Reads recursively the property-files from the directories and
		returns a list of projects to build.
		
		The list has at least one entry. In the first entry are all the
		files collected which are not assigned to a specific project.
		
		Keywords arguments:
		target	--	directory to be parsed
		tag		--	tagname corresponding to the directory
		"""
		project = ProjectProperties('', 'global')
		
		projects = [project]
		projectStack = [project]
		
		for path, directories, files in os.walk(target):
			# exclude the SVN-directories
			if '.svn' in directories:
				directories.remove('.svn')
			
			locals = self.globals.getLocalProperties(path, target, tag)
			directory = self._parseDirectory(path, locals)
			
			if directory.isProject():
				project = directory.getProject()
				
				projects.append(project)
				projectStack.append(project)
			else:
				project = projectStack[-1]
				while True:
					if not path.startswith(project.path):
						projectStack.pop()
						project = projectStack[-1]
					else:
						break
			
			if directory.isEnabled():
				sources, header = self._parseFiles(path, files, directory, target)
				project.sources += sources
				project.header += header
			else:
				# if the this directory should be excluded, remove all the
				# subdirectories from the list to exclude them as well
				tempDirectories = directories[:]
				for d in tempDirectories:
					directories.remove(d)
		
		return projects
	
	def _parseDirectory(self, path, locals):
		"""
		Parse the properties file in a directory
		
		Keyword arguments:
		path	--	path to the directory
		locals	--	local part of the properties-tree corresponding to the
					current directory
		
		"""
		try:
			filename = os.path.join(path, 'properties.yaml')
			tree = yaml.load(open(filename))
		except yaml.YAMLError, e:
			print "Error in configuration file:", e
			raise PropertyException(e)
		except IOError:
			tree = None
		else:
			if self.verbose:
				print "parse '%s/properties.yaml'" % path
		
		return DirectoryProperties(path, tree, locals, self.globals)
	
	def _parseFiles(self, path, files, directory, target):
		"""
		Parse all files inside a directory and create representations of them
		 
		Keywords arguments:
		path		--	path to the directory
		files		--	list of files
		directory	--	directory representation
		target		--	TODO
		
		"""
		sources = []
		header = []
		for file in files:
			extension = os.path.splitext(file)[1]
			filename = os.path.join(path[len(target):], file)
			abspath = os.path.abspath(os.path.join(path, file))
			
			if extension in self.sourcetypes:
				sources.append(directory.getFile(filename,
												 abspath))
			elif extension  in self.headertypes:
				header.append(directory.getFile(filename,
												abspath))
		return (sources, header)
	
	def getGlobalProperties(self):
		return self.globals


import os
import re

import configuration

# -----------------------------------------------------------------------------
def listify(node):
	return [node,] if (not isinstance(node, list)) else node

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
		parser = configuration.Parser()
		
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
					
					try:
						target = parser.get('build', 'target')
						if not re.match(target, self.env['ARCHITECTURE']):
							# if the this directory should be excluded, remove all the
							# subdirectories from the list to exclude them as well
							tempDirectories = directories[:]
							for d in tempDirectories:
								directories.remove(d)
							continue
					except configuration.ParserException:
						pass
					
					try:
						for item in parser.items('defines'):
							self.defines[item[0]] = item[1]
					except configuration.ParserException:
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

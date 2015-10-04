#!/usr/bin/env python
# -*- coding: utf-8 -*-

from lxml import etree
import os
import sys
import logging

from parser_exception import ParserException
import utils
import type
import event
import component
import container
import domain

#logging.basicConfig(level=logging.DEBUG)
logging.basicConfig(level=logging.WARNING)

class Tree(object):
	"""
	XPCC communication structure
	
	Arguments:
	types		-- List of all types
	events		-- List of all events
	components	-- List of all components
	container	-- List of all container
	domains		-- List of all domains
	"""
	def __init__(self):
		self.types = utils.SingleAssignDictionary("types")
		self.events = utils.SingleAssignDictionary("events")
		self.components = component.ComponentDictionary("components")
		self.container = utils.SingleAssignDictionary("container")
		self.domains = utils.SingleAssignDictionary("domain")
	
	def dump(self):
		output = "Types:\n"
		for element in self.types:
			output += "- %s\n" % element
		output += "\nEvents:\n"
		for element in self.events:
			output += "- %s\n" % element
		output += "\nComponents:\n"
		for element in self.components:
			output += "- %s\n" % element
		output += "\nContainer:\n"
		for element in self.container:
			output += "- %s\n" % element
		output += "\nDomains:\n"
		for element in self.domains:
			output += "- %s\n" % element
		return output

class DTDResolver(etree.Resolver):
	def __init__(self, dtdPath):
		self.dtdPath = dtdPath
		
	def resolve(self, url, id, context):
		if os.path.isfile(url):
			return None
		else:
			filename = os.path.basename(url)
			if (self.dtdPath == None):
				self.dtdPath = '.'
			return self.resolve_filename(os.path.join(self.dtdPath, filename), context)

class Parser(object):
	"""
	XPCC XML parser class
	
	Attributes:
	tree		-- Parsed tree of the communication structure
	modify_time	-- Time of the last change of the xml-files
	"""
	def __init__(self):
		self.tree = Tree()
		self.modify_time = 0
	
	def parse(self, filename, dtdPath = '.', include_paths = []):
		"""
		Parse a XML-file
		
		At first all files chained by the include-tag are read in recursively.
		
		Parsing is done then in multiple steps:
		 1. Parse all available types without evaluating
		 2. Evaluate the types
		 3. Parse all events
		 4. Parse all components without evaluating (an empty named component will be placed into the tree)
		 5. Evaluate all components
		 6. Parse all container
		 7. Parse all domains
		
		Step 1 and 4 are necessary because a type/component may reference to
		another type/component with is not parsed at the moment. This
		way is ensured that at least the shell of the objects exists.
		
		After all a checking and optimizing step is performed
		
		Keyword arguments:
		filename	-- xml file to load
		"""
		self.rootfile = filename
		self.dtdPath = dtdPath
		# first include path is always the root file path
		self.include_paths = [os.path.dirname(os.path.abspath(self.rootfile))]
		# additional paths can be specified through the include_paths argument
		self.include_paths = self.include_paths + include_paths
		# used to make sure files are only included once, this has basically
		# the same effect that "#include guards" in C++ have
		self.included_files = []
		
		#all xml file content is stored as documents here in the included-first order
		self.xml_documents = []
		
		
		self._read_and_validate_files(filename)
		
		for d in self.xml_documents:
			self._parse_document(d)
		
		self._check_everything()
		
	def _read_and_validate_files(self, filename):
		"""
		Read all xml files that are reacheble by the include chain
		add their paths to self.included_files for not including files twice
		append them to self.xml_documents in included first order to xml_documents
		"""
		logging.debug("Parse %s" % filename)
		
		self.included_files.append(filename)
		
		try:
			# read the time of the last change
			self.modify_time = max(self.modify_time, os.stat(filename).st_mtime)
			
			parser = etree.XMLParser(dtd_validation=True, load_dtd=True)
			
			# Dynamically resolve DTD paths
			parser.resolvers.add( DTDResolver(dtdPath = self.dtdPath) )
			
			# parse the xml-file
			document = etree.parse(filename, parser)
		except OSError as e:
			raise ParserException(e)
		except Exception as e:
			raise ParserException("Error while parsing xml-file '%s': %s" % (filename, e))
		else:
			logging.debug("Parsing and Validation OK!")
		
		# search for include and reference nodes and parse
		# the specified files first
		xmltree = document.getroot()
		for node in xmltree.findall('include'):
			include_file = Parser.find_include_file(node.text, filename, self.include_paths)
			if include_file not in self.included_files:
				self._read_and_validate_files(include_file)
			else:
				logging.debug("'%s' already included." % include_file)

		self.xml_documents.append(document)
		
		
	def _parse_document(self, xmldocument):
		"""
		"""
		
		xmltree = xmldocument.getroot()
		try:
			self._parse_types(xmltree)
			self._evaluate_types()
			self._create_type_hierarchy()

			self._parse_events(xmltree)
			self._evaluate_events()
			self._check_events()

			self._parse_components(xmltree)
			self._evaluate_components()

			self._parse_container(xmltree)

			self._parse_domains(xmltree)
		except ParserException as e:
			# Add file information that is not available in the lower classes
			# to exception. See:
			# http://www.ianbicking.org/blog/2007/09/re-raising-exceptions.html
			e.args = ("'%s': %s" % (xmldocument.docinfo.URL, e.message),) + e.args[1:0]
			raise
		
	def _check_everything(self):
		# create expanded versions for all types and components
		for t in self.tree.types:
			t.flattened()
		for component in self.tree.components:
			component.flattened()
		self.tree.components.updateIndex()
		
		for container in self.tree.container:
			container.updateIndex()

	@staticmethod
	def find_include_file(filename, include_file, include_paths, line_count=""):
		""" Tries to find the include file and return it's absolute path """
		
		# 1.) include file name can be absolut
		if os.path.isabs(filename):
			return filename
		
		# 2.) it could be a path relative to the files path
		#     this works just like #include "{filename}" in C/C++
		relative_to_file = os.path.abspath(os.path.join(os.path.dirname(include_file), filename))
		if os.path.isfile(relative_to_file):
			return relative_to_file
		
		# 3.) it could be a path relative to the include path
		for path in include_paths:
			relative_to_include_path = os.path.abspath(os.path.join(path, filename))
			if os.path.isfile(relative_to_include_path):
				return relative_to_include_path
			
		# 4.) Error!
		raise ParserException("Could not find include file '%s' in '%s:%s'" % (filename, include_file, line_count))

	def _parse_types(self, xmltree):
		self.__parse_body(xmltree, 'builtin', type.BuiltIn, self.tree.types)
		self.__parse_body(xmltree, 'struct', type.Struct, self.tree.types)
		self.__parse_body(xmltree, 'typedef', type.Typedef, self.tree.types)
		self.__parse_body(xmltree, 'enum', type.Enum, self.tree.types)

	def __parse_body(self, xmltree, name, object, list):
		"""
		Helper function to parse all elements of a single type without
		evaluating the content.
		"""
		for node in xmltree.findall(name):
			element = object(node)
			list[element.name] = element
	
	def _evaluate_types(self):
		for type in self.tree.types:
			type.evaluate(self.tree)
	
	def _create_type_hierarchy(self):
		for type in self.tree.types:
			type.create_hierarchy()
	
	def _parse_events(self, xmltree):
		for node in xmltree.findall('event'):
			element = event.Event(node)
			self.tree.events[element.name] = element
	
	def _evaluate_events(self):
		for e in self.tree.events:
			e.evaluate(self.tree)
	
	def _parse_components(self, xmltree):
		self.__parse_body(xmltree, 'component', component.Component, self.tree.components)
	
	def _evaluate_components(self):
		for component in self.tree.components:
			component.evaluate(self.tree)
	
	def _parse_container(self, xmltree):
		for node in xmltree.findall('container'):
			element = container.Container(node, self.tree)
			self.tree.container[element.name] = element
			
	def _parse_domains(self, xmltree):
		for node in xmltree.findall('domain'):
			element = domain.Domain(node, self.tree)
			self.tree.domains[element.name] = element
	
	def _check_events(self):
		eventIds = {}
		for event in self.tree.events:
			id = event.id
			if id in eventIds:
				raise ParserException("Duplicate Event-Identifier, '0x%02x' is used for '%s' and '%s'!"	% 
						(id, event.name, eventIds[id].name))
			else:
				eventIds[id] = event
	
	

# -----------------------------------------------------------------------------
if __name__ == "__main__":
	parser = Parser()
	parser.parse("../../../../season/common/robot.xml", dtdPath="../../../tools/system_design/xml/dtd")
	
	tree = parser.tree
	
	print tree.dump()
	#print tree.types["Track Segment Line"].flattened().dump()
	#print tree.components["driver"].dump()
	#print tree.components["driver"].flattened().dump()
	#print tree.components["driver"].abstract
	
	#print tree.container["drive"].subscriptions

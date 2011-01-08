#!/usr/bin/env python
# -*- coding: utf-8 -*-

import xml.etree.ElementTree as et
import xml
import copy
import os
import sys

import structure.type
import structure.event
import structure.component
import structure.container
import structure.helper

from structure.exception import ParserError

__version__ = "$Id$"

class ComponentIterator:
	
	def __init__(self, list, abstract, reference):
		self.list = list
		self.list.sort()
		
		self.abstract = abstract
		self.reference = reference
	
	def __iter__(self):
		return self
	
	def next(self):
		try:
			while True:
				item = self.list.pop(0)
				if self.abstract != None and item.abstract != self.abstract:
					continue
				if self.reference != None and item.reference != self.reference:
					continue
				return item
		except IndexError:
			raise StopIteration()


class ComponentDict(structure.helper.SingleAssignDict):
	
	def __init__(self,  name):
		structure.helper.SingleAssignDict.__init__(self, name)
		
		self.actions = structure.helper.SortedDict()
		self.subscriptions = {}
	
	def __update_list(self, toplist, list, name):
		"""
		
		name	--	Is only used for error signaling
		"""
		for element in toplist.iter(reference=None):
			if element.name in list:
				other = list[element.name]
				if element.id != other.id:
					raise ParserError(None, "%s '%s' defined twice with " \
						"different identifiers!" % (name, element.name))
			list[element.name] = element
	
	def updateIndex(self):
		for component in self.values():
			self.__update_list(component.actions, self.actions, "action")
			
			for event in component.events.subscribe:
				# append new events to the list
				key = event.name
				componentList = self.subscriptions.get(key, [])
				componentList.append(component)
				self.subscriptions[key] = componentList
	
	def iter(self, abstract=False, reference=False):
		"""
		Generate an iterator.
		
		Keywords arguments:
		abstract	--	Handling of abstract components (components
						that are not used in any container). For None it
						will iterate over all components, for False
						(default value) only over the not-abstract and for
						True accordingly only over the abstract.
		reference	--	analog to abstract.
		"""
		return ComponentIterator(self.values(), abstract, reference)


class Parser:
	""" 
	Parser Class
	
	self.modify_time		- time of the last change of the xml-file
	"""
	def __init__(self, xmlfile, reference=False, use_reference=False, validate=True):
		"""
		Parse the xml-file and save the tree for further evaulation.
		
		Keyword arguments:
		xmlfile		--	file to parse
		reference		--	xml-file should only be used as a reference
							and not included in the output.
		use_reference	--	use the reference-mechanism, if set False
							the reference-tags will be ignored and
							everything included in the output.
		validate	--	validate the input file against the embedded DTD file
		
		"""
		self.filename = xmlfile
		self.reference = reference and use_reference
		self.use_reference = use_reference
		self.validate = validate
		try:
			# read the time of the last change
			self.modify_time = os.stat(xmlfile).st_mtime
			
			# parse the xml-file
			self.xml = et.parse(xmlfile).getroot()
		except OSError, e:
			raise ParserError(e, 'Error on Reading and Parsing xml file %s' % self.filename)
		except xml.parsers.expat.ExpatError, e:
			raise ParserError(e, 'Error on Reading and Parsing xml file %s' % self.filename)
		
		if validate:
			try:
				import lxml.etree		# for validating
				
				# validate against the embedded DTD file
				try:
					parser = lxml.etree.XMLParser(dtd_validation=True)
					tree = lxml.etree.parse(xmlfile, parser)
				except lxml.etree.XMLSyntaxError, e:
					raise ParserError(None, 'Validation error: "' + xmlfile + '": ' + str(e))
			except ImportError, e:
				print "Warning: couldn't load 'lxml' module. No validation done!"
		
	def parse(self):
		"""
		Evaluate the parsed xml-tree
		
		The modify_time is set to the newest modify time of all 
		included xml-files.
		
		"""
		self.types = structure.helper.SingleAssignDict("type")
		self.events = structure.helper.SingleAssignDict("event")
		self.container = structure.helper.SingleAssignDict("container")
		self.components = ComponentDict("component")
		
		# create an empty parse tree
		tree = [self.types, self.events, self.container, self.components]
		
		# start the evaluating of the xml tree
		self._parse(tree)
		
		# create a iterator for all actions, attributes and events
		# off all components
		self.components.updateIndex()
		
		# check the parsed types, components and containers
		for type in self.types.iter(None):
			type.check()
		for component in self.components.iter(None, None):
			component.check()
		for container in self.container.iter(None):
			container.check()
		for event in self.events.iter(None):
			event.check()
		
		# now check for duplicate identifiers
		self.__check_identifier(self.components.iter(abstract=False), "component")
		self.__check_identifier(self.components.actions, "action")
		self.__check_identifier(self.events, "events")
	
	def _parse(self, tree):
		"""
		Evaluate the parsed xml-tree
		
		Keyword arguments:
		tree	--	result of the evaluation so far
		
		"""
		# search for include and reference nodes and parse
		# the specified files first
		for node in self.xml.findall('include'):
			filename = node.text
			if not os.path.isabs(filename):
				filename = os.path.dirname(os.path.abspath(self.filename)) + '/' + filename
			
			p = Parser(filename, reference=self.reference, validate=self.validate)
			tree = p._parse(tree)
			
			self.modify_time = max(self.modify_time, p.modify_time)
		
		for node in self.xml.findall('reference'):
			filename = node.text
			if not os.path.isabs(filename):
				filename = os.path.dirname(os.path.abspath(self.filename)) + '/' + filename
			
			p = Parser(filename, reference=True and self.use_reference, validate=self.validate)
			tree = p._parse(tree)
			
			self.modify_time = max(self.modify_time, p.modify_time)
		
		self.types, self.events, self.container, self.components = tree
		
		# evaluate the xml-tree
		self.__parse_xml('builtin', structure.type.BuiltIn, self.types)
		self.__parse_xml('struct', structure.type.Struct, self.types)
		self.__parse_xml('typedef', structure.type.Typedef, self.types)
		self.__parse_xml('enum', structure.type.Enum, self.types)
		
		self.__parse_xml('event', structure.event.Event, self.events)
		self.__parse_xml('component', structure.component.Component, self.components)
		self.__parse_xml('container', structure.container.Container, self.container)
		
		# set the componentdefinitions from the global component list to
		# container.
		# Raise exception if no componentdefinition found
		for container in self.container.itervalues():
			for component in container.components:
				componentDefinition = self.components[component.name]
				componentDefinition.abstract = False
				
				container.components.replace(component.name, componentDefinition)
			
			container.updateEvents()
		
		# update the type level
		self.__resolve_struct_dependencies()
		
		# update the type level
		self.__create_type_hierarchy()
		
		# resolve the component inheritance structure
		self.__resolve_component_dependencies()
		
		try:
			# replace the placeholders in the component with references to
			# the real events
			for container in self.container.itervalues():
				for component in container.components:
					for event in component.events.publish:
						component.events.publish.replace(event.name, self.events[event.name])
					for event in component.events.subscribe:
						component.events.subscribe.replace(event.name, self.events[event.name])
		except KeyError, e:
			raise ParserError(None, "Unknown event %s. Check your XML file!" % e)
		
		return [self.types, self.events, self.container, self.components]
	
	def __parse_xml(self, name, type, list):
		"""
		Searches in serf.xml for entities named name.
		Creates instance of type from xml for each found and
		adds the instance to list.
		list[name] = new instance
		
		Keyword arguments:
		name	--	name of the Entity in xml tree.
		type	--	Class, which has constructor(name, reference) and
					method _from_xml(node)
		list	--	dict[name:type]
		
		"""
		for node in self.xml.findall(name):
			element = type(	name = node.get('name'), 
									reference = self.reference)
			element._from_xml(node)
			list[element.name] = element
	
	def __check_type_and_size(self, element):
		try:
			type = self.types[element.type.name]
			if type._checked:
				element._checked = True
				element.level = type.level + 1
				element.size = type.size
				return True
			else:
				return False
		except KeyError, e:
			raise ParserError(e, "could not find type '%s'" % element.type)
	
	def __check_identifier(self, list, type):
		""" Check for duplicate identifiers """
		tmp = {}
		errorId = None
		for element in list:
			if element.id in tmp:
				errorId = element.id
				msg = "Duplicate identifier in %s '%s' and '%s'!" % \
						(type, tmp[element.id].name, element.name)
			else:
				tmp[element.id] = element
		
		if errorId is not None:
			nextId = errorId
			while True:
				nextId = nextId + 1
				if nextId not in tmp:
					break
			raise ParserError(None, msg + " Next available identifier: '0x%02x'." % nextId)
		del tmp
	
	def __create_type_hierarchy(self):
		"""
		Determine the hierarchy of the types
		
		Level-0 types depend only on built-in types (e.g. enum is 
		always level-0), level-1 types depends only on built-in types
		and level-0 types and so on.
		
		Updates the 'level' and '_checked' attributes of each type on
		the self.types list.
		
		Raises a ParserError exception if a loop in the type definition
		is detected.
		
		"""
		while True:
			# every cycle we check at least one hierarchy level
			changed = False
			finished = True
			for element in self.types.itervalues():
				if element._checked:
					continue
				
				finished = False
				if element.isTypedef:
					if self.__check_type_and_size(element):
						changed = True
				elif element.isStruct:
					checked = True
					level = -1
					sizeSum = 0
					# check all the subelements of the struct
					# the level of the struct could not be determined
					# before all subelements have their level 
					for subelement in element.iter():
						if subelement._checked or self.__check_type_and_size(subelement):
							level = max(level, subelement.level)
							sizeSum += subelement.size
						else:
							checked = False
							break
					
					if checked:
						element._checked = True
						element.level = level
						element.size = sizeSum
						changed = True
				
			if finished:
				break
			else:
				if changed:
					continue
				
				# the hierarchy of some types couldn't determined
				# until now, but no definition changed in this cycle
				# => at least two types are referencing each other
				
				# check which elements are causing the loop
				types = []
				for element in self.types.itervalues():
					if not element._checked:
						types.append(element.name)
				raise ParserError(None, "Detected loop in type defintion!"
									" Check types '%s'!" %
									"' and '".join(types))
	
	def __update_component(self, component):
		"""
		Update the components recursive 
		"""
		if component.extends:
			top = self.components[component.extends]
			if top.extends:
				self.__update_component(top)
			component.extend(top)
	
	def __resolve_component_dependencies(self):
		"""
		Resolve the dependencies of the component definitions
		and add the action, attributes etc. from the superordinate
		components.
		
		"""
		for component in self.components.itervalues():
			self.__update_component(component)

	def __update_struct(self, struct):
		"""
		Update the components recursive 
		"""
		if struct.extends:
			top = self.types[struct.extends]
			if not top.isStruct:
				raise ParserError(None, "Detected struct which is extended from a non struct!"
					" Check '%s'!" % struct)
			if top.extends:
				self.__update_struct(top)
			struct.extend(top)
	
	def __resolve_struct_dependencies(self):
		"""
		Resolve the dependencies of the struct definitions
		and add the inherited members from the superordinate
		structs.
		
		"""
		for element in self.types.itervalues():
			if element.isStruct:
				self.__update_struct(element)

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	try:
		parser = Parser("../../../common/xml/robot.xml", validate=True)
		parser.parse()
	except ParserError, e:
		print "Error:", e
		sys.exit(1)
#	except Exception, e:
#		print "Eee:", e
#		sys.exit(1)
	
#	print "Components:"
#	for component in parser.components.iter():
#		print component
#		print ""
#	print ""
	
#	print "Events:"
#	for event in parser.events.iter():
#		print event
#	print ""
	
	print "Types:"
	for packet in parser.types.iter():
		print packet
		print ""

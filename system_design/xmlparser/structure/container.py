#!/usr/bin/env python
# -*- coding: utf-8 -*-

import helper
import component
from component_element import EventContainer

class Container:
	""" Representation of a container which bundles some components.
	"""
	def __init__(self, name, reference=False):
		self.name = name
		self.description = None
		self.bootloader = None
		
		self.reference = reference
		self.components = helper.SingleAssignDict("component")
		self.events = EventContainer()
	
	def check(self):
		helper.check_name_notation(self, self.name)
	
	def add(self, component):
		""" Append an component to the container """
		self.components[component.name] = component
	
	def updateEvents(self):
		#for component in self.components:
		#	self.events.update(component.events)
		pass
	
	def _from_xml(self, node):
		bootloader = node.find('bootloader')
		if bootloader != None:
			bootloader = bootloader.attrib
		
		self.description = helper.xml_read_description(node), 
		self.bootloader = bootloader
		
		self.extended = helper.xml_read_extended(node)
		
		for subnode in node.findall('component'):
			subcomponent = component.Component(
									name = subnode.get('name'), 
									reference = self.reference)
			subcomponent._from_xml(subnode)
			
			self.add(subcomponent)
	
	def __cmp__(self, other):
		return cmp(self.name.lower(), other.name.lower())
	
	def __str__(self):
		str = "container: %s\n" % self.name
		for component in self.components:
			str += "- %s\n" % '\n'.join(["  " + line for line in component.__str__().split('\n')])[2:]
		return str[:-1]

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	container = Container("container")
	
	print container

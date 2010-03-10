#!/usr/bin/env python
# -*- coding: utf-8 -*-

import helper
import component

class Board:
	"""
	Representation of a board which contains some components.
	
	"""
	def __init__(self, name, reference=False):
		self.name = name
		self.description = None
		self.src_dir = None
		self.bootloader = None
		
		self.reference = reference
		self.components = helper.SingleAssignDict("component")
	
	def check(self):
		helper.check_name_notation(self, self.name)
	
	def add(self, component):
		""" Append an component to the board """
		self.components[component.name] = component
	
	def _from_xml(self, node):
		bootloader = node.find('bootloader')
		if bootloader != None:
			bootloader = bootloader.attrib
		
		self.description = helper.xml_read_description(node), 
		self.src_dir = helper.xml_read_value(node, 'source')
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
		str = "board: %s\n" % self.name
		for component in self.components:
			str += "- %s\n" % '\n'.join(["  " + line for line in component.__str__().split('\n')])[2:]
		return str[:-1]

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	board = Board("board")
	
	print board

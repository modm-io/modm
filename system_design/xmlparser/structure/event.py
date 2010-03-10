#!/usr/bin/env python
# -*- coding: utf-8 -*-

import helper

class Event:
	"""
	Representation of a board which contains some components.
	
	"""
	def __init__(self, name, reference=False):
		self.name = name
		
		self.id = -1
		self.description = None
		self.type = None
		self.rate = None
		
		self.reference = reference
	
	def check(self):
		helper.check_name_notation(self, self.name)
	
	def _from_xml(self, node):
		self.id = helper.xml_read_identifier(node)
		
		self.description = helper.xml_read_description(node), 
		self.extended = helper.xml_read_extended(node)
		
		self.type = node.get('type')
		self.rate = node.get('rate')
	
	def __cmp__(self, other):
		return cmp(self.id, other.id) or cmp(self.name, other.name)
	
	def __str__(self):
		return "[%02x] %s : %s" % (	self.id,
									self.name.replace(" ", ""),
									self.type)

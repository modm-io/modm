#!/usr/bin/env python
# -*- coding: utf-8 -*-

import utils
import xml_utils

from parser_exception import ParserException

class Event(object):
	
	def __init__(self, node):
		""" Constructor
		
		Keyword arguments:
		node	--	XML node defining this event
		"""
		self.node = node
		
		self.name = node.get('name')
		utils.check_name(self.name)
		
		self.id = None
		self.description = None
		self.rate = None
		self.type = None
		
	
	def evaluate(self, tree):
		if self.node is None:
			return
		
		self.id = xml_utils.get_identifier(self.node)
		self.description = xml_utils.get_description(self.node)
		self.rate = self.node.get('rate')
		
		type = self.node.get('type')
		if type is None:
			self.type = None
		else:
			try:
				self.type = tree.types[type]
			except KeyError as e:
				raise ParserException("Type '%s' is not defined. Used by Event '%s')" % (type, self.name))
			
		self.node = None
		
	def update(self, other):
		""" Update events with the values from another event
		
		Events are guaranteed to be unique within the evaluted tree. Therefore
		an update demand can only be issued for the same events, one declared
		in the super-class and the other in the sub-class.
		
		The assert statement checks this, nothing else needs to be done.
		"""
		assert id(self) == id(other)
		
	def __cmp__(self, other):
		return cmp(self.id, other.id) or cmp(self.name, other.name)
	
	def __str__(self):
		if self.type is None:
			type = None
		else:
			type = self.type.name
		return "[%02x] %s : %s" % (self.id, self.name, type)

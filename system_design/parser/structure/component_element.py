#!/usr/bin/env python
# -*- coding: utf-8 -*-

import helper

class ComponentElement(object):
	
	def __init__(self, name, reference=False):
		self.name = name
		self.reference = reference
		
		self.implementation = {}
	
	def get_type(self):
		return None
	
	def check(self):
		helper.check_name_notation(self, self.name)
	
	def update(self, top):
		assert self.name == top.name
		for key, value in self.__dict__.items():
			if value == None:
				self.__dict__[key] = top.__dict__[key]
	
	def _from_xml(self, node):
		self.desc = helper.xml_read_description(node)
		self.id = helper.xml_read_identifier(node)
		
		self.implementation = helper.xml_read_implementation(node)


class Action(ComponentElement):
	
	def __init__(self, name, reference=False):
		ComponentElement.__init__(self, name, reference)
		
		self.desc = None
		self.id = -1
		self.function = None
		self.param_type = None
		self.ret_type = None
	
	def get_type(self):
		return "action"
	
	def _from_xml(self, node):
		ComponentElement._from_xml(self, node)
		
		self.param_type = node.get('parameterType')
		self.ret_type = node.get('returnType')
	
	def __cmp__(self, other):
		return cmp(self.id, other.id) or cmp(self.name, other.name)
	
	def __str__(self):
		param_type = self.param_type or ""
		ret_type = (" : %s" % self.ret_type) if (self.ret_type) else ""
		return "[%02x] %s(%s)%s" % (	self.id,
										self.name.replace(" ", "_"),
										param_type,
										ret_type)


class Event(ComponentElement):
	
	def __init__(self, name, reference=False):
		ComponentElement.__init__(self, name, reference)
		
		self.desc = None
		self.id = -1
		self.type = None
		self.rate = None
	
	def get_type(self):
		return "event"
	
	def _from_xml(self, node):
		ComponentElement._from_xml(self, node)
		
		self.type = node.get('type')
		self.rate = node.get('rate')
	
	def __cmp__(self, other):
		return cmp(self.id, other.id) or cmp(self.name, other.name)
	
	def __str__(self):
		return "[%02x] %s : %s" % (	self.id,
										self.name.replace(" ", "_"),
										self.type) 

#!/usr/bin/env python
# -*- coding: utf-8 -*-

import helper

class EventContainer:
	def __init__(self):
		self.publish = helper.SingleAssignDict("event::publish")
		self.subscribe = helper.SingleAssignDict("event::subscribe")
	
	def update(self, other):
		self.publish.update(other.publish)
		self.subscribe.update(other.subscribe)
	
	def __str__(self):
		return "event::publish\n%s\nevent::subscribe\n%s" % (str(self.publish), str(self.subscribe))


class ComponentElement(object):
	
	def __init__(self, name, reference=False):
		self.name = name
		self.reference = reference
		
		self.extended = {}
	
	def check(self):
		helper.check_name_notation(self, self.name)
	
	def update(self, top):
		assert self.name == top.name
		for key, value in self.__dict__.items():
			if value == None:
				self.__dict__[key] = top.__dict__[key]
	
	def _from_xml(self, node):
		self.description = helper.xml_read_description(node)
		self.id = helper.xml_read_identifier(node)
		
		self.extended = helper.xml_read_extended(node)


class Action(ComponentElement):
	
	def __init__(self, name, reference=False):
		ComponentElement.__init__(self, name, reference)
		
		self.description = None
		self.id = -1
		self.parameterType = None
		self.returnType = None
	
	def _from_xml(self, node):
		ComponentElement._from_xml(self, node)
		
		self.parameterType = node.get('parameterType')
		self.returnType = node.get('returnType')
	
	def __cmp__(self, other):
		return cmp(self.id, other.id) or cmp(self.name, other.name)
	
	def __str__(self):
		parameterType = self.parameterType or ""
		returnType = (" : %s" % self.returnType) if (self.returnType) else ""
		return "[%02x] %s(%s)%s" % (	self.id,
										self.name.replace(" ", ""),
										parameterType,
										returnType)


class Event(ComponentElement):
	
	def __init__(self, name, reference=False):
		ComponentElement.__init__(self, name, reference)
	
	def _from_xml(self, node):
		pass
	
	def __cmp__(self, other):
		return cmp(self.name, other.name)
	
	def __str__(self):
		return "Event %s" % self.name.replace(" ", "_") 

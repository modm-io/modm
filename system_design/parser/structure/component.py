#!/usr/bin/env python
# -*- coding: utf-8 -*-

import helper
from exception import ParserError
from component_element import Action, Event

class Component(object):
	"""
	Representation of a component.
	"""
	def __init__(self, name, reference=False):
		self.name = name
		self.desc = None
		
		self.id = None
		self.extends = None
		self.group = None
		
		self.reference = reference
		self.abstract = True
		
		self.actions = helper.SingleAssignDict("action")
		self.events = helper.SingleAssignDict("event")
	
	def check(self):
		# check own values
		helper.check_name_notation(self, self.name)
		
		# check the action, attributes and events
		for list in [self.actions, self.attributes, self.events]:
			for element in list:
				element.check()
	
	def extend(self, top):
		self.extends = top.extends
		
		# update the attributes if they are not None
		self.desc = self.desc or top.desc
		self.id = self.id or top.id
		self.group = self.group or top.group
		
		# update actions, attributes and events
		self.__extend(top.actions, self.actions)
		self.__extend(top.events, self.events)
	
	def __extend(self, toplist, list):
		for element in toplist.iter(reference=None):
			try:
				# try to update an already existing element
				# from this component with the values from the
				# toplevel component
				list[element.name].update(element)
			except KeyError:
				# no element found, inherit the full top element
				list[element.name] = element
	
	def add(self, element):
		list = {
			"action": self.actions, 
			"event": self.events
		}[element.get_type()]
		list[element.name] = element
	
	def _from_xml(self, node):
		self.desc = helper.xml_read_description(node)
		self.id = helper.xml_read_identifier(node)
		
		self.extends = node.get('extends')
		self.group = node.get('group')
		
		# parse functions, attributes and events
		self.__parse_element('action', node, Action)
		self.__parse_element('event', node, Event)
		
		self.implementation = helper.xml_read_implementation(node)
	
	def __parse_element(self, name, node, elemtype):
		for subnode in node.findall(name):
			element = elemtype(	name = subnode.get('name'), 
									reference = self.reference)
			element._from_xml(subnode)
			self.add(element)
	
	def __cmp__(self, other):
		return cmp(self.id, other.id) or cmp(self.name, other.name)
	
	def __str__(self):
		"""
		Print a nice UML-like box of content of the component
		
		Example:
		/================================================\
		|               Stage Object [ee]                |
		|------------------------------------------------|
		| +[00] ping()                                   |
		| +[01] reset()                                  |
		| +[03] save_configuration()                     |
		|------------------------------------------------|
		| +[02] error : Error Payload                    |
		| +[0f] debug_message : char[]                   |
		\================================================/
		
		The numbers in the square brackets shows the identifier (in
		hexadecimal). First block shows the actions and the second the events.
		
		"""
		name = "%s [%02x]" % (self.name, self.id)
		max_length = len(name)
		
		elements = [[], [], []]
		for i, iterator in enumerate([self.actions, self.events]):
			for element in iterator.iter(reference=None):
				string = "+%s" % element
				max_length = max(max_length, len(string))
				elements[i].append(string)
		
		output  = "/=" + "=" * max_length + "=\\\n"
		
		length = (max_length - len(name)) / 2.0
		pre = " " * int(length)
		post = pre + (" " if (length != int(length)) else "")
		output += "| " + pre + name + post + " |\n"
		
		for element in elements:
			output += "|-" + "-" * max_length + "-|\n"
			for string in element:
				length = max_length - len(string)
				output += "| " + string + " " * length + " |\n"
		
		output += "\\=" + "=" * max_length + "=/\n"
		return output[:-1]


# -----------------------------------------------------------------------------
if __name__ == '__main__':
	component = Component("test1")
	
	component2 = Component("test2")
	component2.add_action(Action("action1"))
	
	component.extend(component2)
	
	print component
	
	action_top = Action("action", ret_type="ret_type")
	
	action = Action("action")
	action.update(action_top)
	
	print action
	print action.__dict__

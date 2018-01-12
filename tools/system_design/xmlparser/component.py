#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2011, Fabian Greif
# Copyright (c) 2015, Niklas Hauser
# Copyright (c) 2015-2016, Georgi Grinshpun
# Copyright (c) 2015-2016, Sascha Schade
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import copy

from . import utils
from . import action
from . import xml_utils
from .parser_exception import ParserException

class ComponentDictionary(utils.SingleAssignDictionary):
	
	def __init__(self,  name):
		utils.SingleAssignDictionary.__init__(self, name)
		
		self.actions = utils.SortedDictionary()
		self.subscriptions = {}
	
	def updateIndex(self):
		""" Update the list of all actions and checks for duplicate identifiers """
		for component in self.values():
			for element in component.flattened().actions:
				if element.name in self.actions:
					other = self.actions[element.name]
					if element.id != other.id:
						raise ParserException("%s '%s' defined twice with " \
							"different identifiers!" % (self.name, element.name))
				self.actions[element.name] = element
			
			for event in component.flattened().events.subscribe:
				# append new events to the list
				key = event.name
				componentList = self.subscriptions.get(key, [])
				componentList.append(component)
				self.subscriptions[key] = componentList
		
		# check for duplicate identifiers
		actionIds = {}
		for action in self.actions:
			id = action.id
			if id in actionIds:
				next_id = self._find_next_available_action_id(id)
				
				raise ParserException("Duplicate Action-Identifier, '0x%02x'. Used for '%s' and '%s'," \
						"Next unused: '0x%02x'."	% 
						(id, action.name, actionIds[id].name, next_id))
			else:
				actionIds[id] = action
		
		# check that the component identifier is unique
		componentIdentifier = {}
		for component in self.values():
			id = component.flattened().id
			
			if id is None:
				continue
			
			if id in componentIdentifier:
				next_id = self._find_next_available_component_id(id)
				
				raise ParserException("Duplicate Component-Identifier: '0x%02x'. Used for '%s' and '%s'. " \
						"Next unused: '0x%02x'."	% 
						(id, component.name, componentIdentifier[id].name, next_id))
			else:
				componentIdentifier[id] = component
	
	def _find_next_available_component_id(self, id):
		"""Returns the next higher component id that is not used."""
		
		#go through all availiable components and store their ids in ci
		#find the next id, that is available
		usedIds = []
		for component in self.values():
			i = component.flattened().id
			if i is not None:
				usedIds.append(i)
		
		while id in usedIds:
			id += 1
			
		return id
	
	def _find_next_available_action_id(self, id):
		"""Returns the next higher action id that is not used."""
		
		#go through all availiable actions and store their ids in
		#find the next id, that is available
		usedIds = []
		for action in self.actions:
			usedIds.append(action.id)
		
		while id in usedIds:
			id += 1
			
		return id
	
	def __iter__(self):
		""" Generates an iterator that will iterate over all non abstract components """
		return self.iter(None)
	
	def iter(self, abstract=False):
		"""
		Generate an iterator.
		
		Keywords arguments:
		abstract	--	Handling of abstract components (components
						that are not used in any container). For None it
						will iterate over all components, for False
						(default value) only over the none-abstract and for
						True accordingly only over the abstract.
		"""
		class ComponentIterator:
			def __init__(self, list, abstract):
				self.list = sorted(list)
				self.abstract = abstract
			
			def __iter__(self):
				return self

			def __next__(self):
				return self.next()

			def next(self):
				try:
					while True:
						item = self.list.pop(0)
						if self.abstract is not None and item.abstract != self.abstract:
							continue
						return item
				except IndexError:
					raise StopIteration()
		
		return ComponentIterator(self.values(), abstract)

class EventContainer(object):
	"""
	
	Attributes:
	publish		--	All events the component will publish
	subscribe	--	All events the component will receive
	"""
	def __init__(self):
		self.publish = utils.SingleAssignDictionary("event::publish")
		self.subscribe = utils.SingleAssignDictionary("event::subscribe")
	
	def update(self, other):
		self.publish.update(other.publish)
		self.subscribe.update(other.subscribe)
	
	def copy(self):
		""" Create a shallow copy """
		c = copy.copy(self)
		c.publish = self.publish.copy()
		c.subscribe = self.subscribe.copy()
		return c
	
	def __str__(self):
		return "event::publish:\n%s\n"\
			   "event::subscribe:\n%s" % (str(self.publish), str(self.subscribe))

class Component(object):
	""" Representation of a component.
	
	Attributes:
	name		--	Name of the component
	description	--	Description string
	id			--	Identifier of the component (0..255). Must be set for
					any non abstract component. Can be 'None' for abstract
					components.
	extends		--	Super component. 'None' if no super component present.
	abstract	--	'False' if this component is used in any container. Default
					value is 'True', might be set to 'False' when parsing
					the containers.
	actions		--	List of actions
	events		--	List of used events
	"""
	def __init__(self, node):
		self.node = node
		self.name = node.get('name')
		utils.check_name(self.name)
		self.description = None
		
		self.id = None
		self.extends = None
		self.abstract = True
		
		self.__flattened = None
		
		self.actions = utils.SingleAssignDictionary("action")
		self.events = EventContainer()
	
	def evaluate(self, tree):
		""" Load values form the XML node. """
		if self.node is None:
			return
		
		self.description = xml_utils.get_description(self.node)
		self.id = xml_utils.get_identifier(self.node)
		
		extends = self.node.get('extends')
		if extends is None:
			self.extends = None
		else:
			try:
				self.extends = tree.components[extends]
			except KeyError:
				raise ParserException("Found unknown super component '%s' in component '%s'!" % (extends, self.name))
		
		# parse functions and events
		self.__parse_actions(tree)
		self.__parse_events(tree)
		
		self.node = None
	
	def __parse_actions(self, tree):
		node = self.node.find('actions')
		if node == None:
			return
		
		for subnode in node.findall('action'):
			element = action.Action(subnode, tree)
			self.actions[element.name] = element
	
	def __parse_events(self, tree):
		event_node = self.node.find('events')
		if event_node == None:
			return
		
		nodes = event_node.find('publish')
		if nodes is not None:
			self.__parse_event(nodes, self.events.publish, tree)
		
		nodes = event_node.find('subscribe')
		if nodes is not None:
			self.__parse_event(nodes, self.events.subscribe, tree)
	
	def __parse_event(self, node, list, tree):
		for node in node.findall('event'):
			event = node.get('name')
			try:
				list[event] = tree.events[event]
			except KeyError:
				raise ParserException("Found unknown event '%s' in component '%s'!" % (event, self.name))
	
	@property
	def resumables(self):
		return len([a for a in self.actions if a.call == "resumable"])

	def flattened(self):
		""" Access the version with the flattened hierarchy
		
		In this version the information about the inheritance relations will
		be lost. But the normal version will still contain them.
		
		During the expanding of all super components the value of
		self.__flattened will be used for loop detection. If the value is -1
		it means that during the flattening of the super components our
		component was reached again => unresolveable cycle in the hierarchy tree!
		"""
		if self.extends is None:
			return self
		elif self.__flattened is not None:
			return self.__flattened
		elif isinstance(self.__flattened, int) and self.__flattened == -1:
			raise ParserException("Loop in the definition of component '%s' detected!" % self.name)
		
		self.__flattened = -1
		top = self.extends.flattened()
		
		flat = copy.copy(self)
		flat.actions = flat.actions.copy()
		flat.events = flat.events.copy()
		
		# update missing values with values form the super component
		flat.description = flat.description or top.description
		flat.id = flat.id or top.id
		flat.extends = None
		
		flat.actions.update(top.actions)
		flat.events.update(top.events)
		
		if flat.abstract == False:
			if flat.id is None:
				raise ParserException("No id defined for the non abstract component '%s'!" % flat.name)
		
		self.__flattened = flat
	
	def __cmp__(self, other):
		return 1 - self.__eq__(other) - 2 * self.__lt__(other)

	def __lt__(self, other):
		if self.id == other.id:
			return self.name < other.name
		if self.id is None:
			return other.id is not None
		return other.id is not None and self.id < other.id

	def __eq__(self, other):
		return self.id == other.id and self.name == other.name

	def dump(self):
		"""
		Print a nice UML-like box of content of the component
		
		Example:
		/================================================\
		|               Stage Object [ee]                |
		|------------------------------------------------|
		| actions:                                       |
		| +[00] ping()                                   |
		| +[01] reset()                                  |
		| +[03] save_configuration()                     |
		|------------------------------------------------|
		| publish:                                       |
		| +[02] error : Error Payload                    |
		| +[0f] debug_message : char[]                   |
		|------------------------------------------------|
		| subscribe:                                     |
		| +[02] error : Error Payload                    |
		\================================================/
		
		The numbers in the square brackets shows the identifier (in
		hexadecimal). First block shows the actions and the second the events.
		
		"""
		name = self.__str__()
		max_length = len(name)
		
		elements = [["actions:"], ["publish:"], ["subscribe:"]]
		for element in elements:
			max_length = max(max_length, len(element[0]))
		
		for i, list in enumerate([self.actions, self.events.publish, self.events.subscribe]):
			for element in list:
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
	
	def __str__(self):
		if self.id is None:
			name = "[--] %s" % self.name
		else:
			name = "[%02x] %s" % (self.id, self.name)
		return name

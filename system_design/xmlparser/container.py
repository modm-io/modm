#!/usr/bin/env python
# -*- coding: utf-8 -*-

import utils
import xml_utils
from component import EventContainer

from parser_exception import ParserException

class Container:
	""" Representation of a container which bundles components.
	
	Attributes:
	name			--	Name of the container
	bootloader		--	Information about a bootloader used to program this
						container.
	description		--	Description string
	components		--	Components implemented in this container
	events			--	All events use by the components in this container
	subscriptions	--	Dictionary about which event is subscribed by
						which component. Needed for example to generate filters
						for a container.
	
	The events and subscriptions lists are empty until createEventLists() is
	called. This must be done only after all components are expanded.
	"""
	def __init__(self, node, tree):
		""" Constructor
		
		Keyword arguments:
		node	--	XML node defining this event
		tree	--	currently evaluted communication structure tree
		
		Resets the 'abstract' flag for every component beeing contained here.
		"""
		self.name = node.get('name')
		utils.check_name(self.name)
		
		bootloader = node.find('bootloader')
		if bootloader != None:
			bootloader = bootloader.attrib
		self.bootloader = bootloader
		
		self.description = xml_utils.get_description(node)
		
		self.components = utils.SingleAssignDictionary("component")
		self.events = EventContainer()
		self.subscriptions = {}
		self.indexReady = False
		
		for node in node.findall('component'):
			component_name = node.get('name')
			try:
				component = tree.components[component_name]
				component.abstract = False
				
				self.components[component_name] = component
			except KeyError:
				raise ParserException("Unknown component '%s' in container '%s'" % (component, self.name))
	
	def updateIndex(self):
		""" Create the lists of all events use by the components
		in this container """
		if self.indexReady:
			return
		
		for component in self.components:
			self.events.update(component.flattened().events)
			
			for event in component.flattened().events.subscribe:
				# append new events to the list
				key = event.name
				componentList = self.subscriptions.get(key, [])
				componentList.append(component)
				self.subscriptions[key] = componentList
		
		self.indexReady = True
	
	def __cmp__(self, other):
		return cmp(self.name.lower(), other.name.lower())
	
	def dump(self):
		str = "%s : container\n" % self.name
		for component in self.components:
			str += "- %s\n" % '\n'.join(["  " + line for line in component.__str__().split('\n')])[2:]
		return str[:-1]
	
	def __str__(self):
		return self.name

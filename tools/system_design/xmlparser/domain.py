#!/usr/bin/env python
# -*- coding: utf-8 -*-

import utils
import xml_utils

from parser_exception import ParserException

class Domain:
	""" Representation of a domain ids.
	
	Attributes:
	name			--	Name of the domain
	description		--	Description string
	id				--	ID of the domains
	"""
	def __init__(self, node, tree):
		""" Constructor
		
		Keyword arguments:
		node	--	XML node defining this event
		tree	--	currently evaluted communication structure tree
		
		"""
		self.name = node.get('name')
		utils.check_name(self.name)

		self.id = xml_utils.get_identifier(node)
		
		self.description = xml_utils.get_description(node)
				
	def __cmp__(self, other):
		return cmp(self.name.lower(), other.name.lower())
	
	def dump(self):
		return "Domain %s\n" % self.name
	
	def __str__(self):
		return "[%02x] %s" % (self.id, self.name)

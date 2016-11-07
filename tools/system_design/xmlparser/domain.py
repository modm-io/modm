#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, Martin Rosekeit
# Copyright (c) 2015, Georgi Grinshpun
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

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
	def __init__(self, node):
		""" Constructor
		
		Keyword arguments:
		node	--	XML node defining this event
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

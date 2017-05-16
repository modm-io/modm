#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2011, Fabian Greif
# Copyright (c) 2011, Georgi Grinshpun
# Copyright (c) 2015, Niklas Hauser
# Copyright (c) 2015, Sascha Schade
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

class Action(object):
	
	def __init__(self, node, tree):
		self.name = node.get('name')
		utils.check_name(self.name)
		
		self.description = xml_utils.get_description(node)
		self.id = xml_utils.get_identifier(node)
		
		self.parameterType = self.__get_type(node, "parameterType", tree)
		self.returnType = self.__get_type(node, "returnType", tree)

		self.call = node.get('call')
		if self.call not in ["once", "resumable"]:
			self.call = "once"

	def __get_type(self, node, name, tree):
		type = node.get(name)
		if type is not None:
			try:
				type = tree.types[type]
			except KeyError as e:
				raise ParserException("Type '%s' is not defined. Used by Action '%s')" % (type, self.name))
				
		return type
	
	def __cmp__(self, other):
		return cmp(self.id, other.id) or cmp(self.name, other.name)
	
	def update(self, top):
		assert self.name == top.name
		for key, value in self.__dict__.items():
			if value == None:
				self.__dict__[key] = top.__dict__[key]
	
	def __str__(self):
		parameterType = self.parameterType or ""
		returnType = (" : %s" % self.returnType) if (self.returnType) else ""
		return "[%02x] %s(%s)%s" % (self.id,
									self.name.replace(" ", ""),
									parameterType,
									returnType)

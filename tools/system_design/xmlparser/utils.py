#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2011, Fabian Greif
# Copyright (c) 2011, 2015, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import re
import copy

from parser_exception import ParserException

def check_name(name):
	""" Checks if a string comply with some rules for the notation
	of a name.
	
	"""
	if not re.match("^([A-Z]?[a-z0-9]* )*([A-Z]?[a-z0-9]*)$", name):
		raise ParserException("Error in name '%s'. Check spelling or case. Names must be 'som How spaced Style' matching '^([A-Z]?[a-z0-9]* )*([A-Z]?[a-z0-9]*)$' " % name)

class SortedDictionary(dict):
	"""
	A dictionary with an Iterator which sorts the output
	"""
	def __iter__(self):
		class Iterator:
			def __init__(self, list):
				self.list = list
				self.list.sort()
			
			def __iter__(self):
				return self
			
			def next(self):
				try:
					item = self.list.pop(0)
					return item
				except IndexError:
					raise StopIteration()
		
		return Iterator(self.values())
	
	def iter(self):
		return self.__iter__()

class SingleAssignDictionary(SortedDictionary):
	""" A dictionary which don't allow overwriting attributes after
	the initial creation.
	"""
	def __init__(self,  name):
		self.name = name
		SortedDictionary.__init__(self)
	
	def __setitem__(self, key, item):
		if not self.has_key(key):
			SortedDictionary.__setitem__(self, key, item)
		else:
			raise ParserException("%s '%s' defined twice!" % (self.name.capitalize(), key))
	
	def remove(self, key):
		SortedDictionary.pop(self, key)
	
	def replace(self, key, item):
		SortedDictionary.__setitem__(self, key, item)
	
	def update(self, other):
		for element in other:
			try:
				# try to update an already existing element
				# from this component with the values from the
				# toplevel component
				SortedDictionary.__getitem__(self, element.name).update(element)
			except KeyError:
				# no element found, inherit the full top element
				SortedDictionary.__setitem__(self, element.name, element)
	
	def copy(self):
		""" Create a shallow copy """
		return copy.copy(self)

#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import copy

from parser_exception import ParserException

def check_name(name):
	""" Checks if a string comply with some rules for the notation
	of a name.
	
	"""
	if not re.match("^([A-Z]?[a-z0-9]* )*([A-Z]?[a-z0-9]*)$", name):
		raise ParserException("Error in name '%s'. Check Spelling or Case. Allowed are only Names matching '^([A-Z]?[a-z0-9]* )*([A-Z]?[a-z0-9]*)$' " % name)

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

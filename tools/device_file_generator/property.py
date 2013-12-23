# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

import os, sys

from identifiers import Identifiers

# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger
# add python module device files to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))
from device_identifier import DeviceIdentifier

class Property:
	""" Property
	Encapsulates a device property.
	This manages merging and accessing of properties.
	"""

	def __init__(self, id, name, value, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		self.name = name
		self.values = [PropertyValue(id, value, self.log)]
	
	def addValue(self, other):
		if isinstance(other.value, list):
			for prop in self.values:
				# create intersection of both lists
				intersection = [val for val in other.value if val in prop.value]
				# if the intersection includes at least on item in other.value
				# add the id and continue
				if len(intersection) > 0:
					prop.ids.extend(other.ids)
					# remove items from other_value that are also in the intersection
					other.value = [v for v in other.value if v not in intersection]
				if len(other.value) == 0:
					return
			
		else:
			for prop in self.values:
				if (prop.value == other.value):
					prop.ids.extend(other.ids)
					return
		
		self.values.append(other)
		self.values.sort(key=lambda k : k.value)
	
	def getMergedProperty(self, other):
		assert isinstance(other, Property)
		assert other.name == self.name
		
		for value in other.values:
			self.addValue(value)
		
		return self
	
	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return ("Property(name='%s', values=[\n%s ])" % (self.name, \
					",\n".join([str(value) for value in self.values]))) \
					.replace('\n', '\n\t')


class PropertyValue:
	""" PropertyValue
	Encapsulates a device property value with ids.
	"""

	def __init__(self, id, value, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		self.ids = Identifiers(id, self.log)
		self.value = value
	
	@property
	def id(self):
		return self.ids.intersection
	
	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return ("PropertyValue(value='%s', ids= %s )" % (self.value, self.ids))

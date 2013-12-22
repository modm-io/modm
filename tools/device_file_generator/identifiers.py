# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

import os, sys
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger
# add python module device files to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))
from device_identifier import DeviceIdentifier

class Identifiers:
	""" Identifiers
	Encapsulates a list of device identifiers.
	This manages filtering, merging and accessing of device ids.
	"""

	def __init__(self, id=None, logger=None):
		if isinstance(id, Identifiers):
			self.log = id.log
			self.ids = id.ids
			return
			
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		self.ids = [DeviceIdentifier(id)]
	
	def append(self, id):
		assert isinstance(id, DeviceIdentifier)
		
		self.ids.append(id)
		self.ids = list(set(self.ids))
		self.ids.sort(key=lambda k : k.string)
		
	def extend(self, ids):
		assert isinstance(ids, Identifiers)
		for id in ids.ids:
			self.ids.append(id)
		self.ids = list(set(self.ids))
		self.ids.sort(key=lambda k : k.string)
	
	def differenceFromIds(self, ids):
		assert isinstance(ids, Identifiers)
		# list all other ids that are not part of our ids
		other_ids = [DeviceIdentifier(id) for id in ids.ids if id not in self.ids]
		
		# create the intersection of all ids
		intersection_id = DeviceIdentifier(ids[0])
		for id in ids[1:]:
			intersection_id = intersection_id.intersectionWithDeviceIdentifier(id)
		
		# strip this information from remaining ids
		for id in other_ids:
			for attr in id.properties:
				if id.properties[attr] == intersection_id.properties[attr]:
					setattr(id, attr, None)
		
# 		print other_ids
		
		return self.union
	
	def __getitem__(self, index):
		return self.ids[index]
	
	def getAttribute(self, name):
		attributes = []
		
		for id in self.ids:
			attributes.append(getattr(id, name))
		
		if len(attributes) == 0:
			return [None]
		
		attributes = list(set(attributes))
		attributes.sort()
		return attributes
	
	
	@property
	def intersection(self):
		dev = DeviceIdentifier(self.ids[0])
		for id in self.ids[1:]:
			dev = dev.intersectionWithDeviceIdentifier(id)
		return dev
	
	@property
	def union(self):
		dev = DeviceIdentifier()
		for id in self.ids:
			dev = dev.unionWithDeviceIdentifier(id)
		return dev
	
	@property
	def string(self):
		return ", ".join([str(id.string) for id in self.ids])

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return ("Identifiers(\n\t%s )" % \
					",\n\t".join(str(id) for id in self.ids))
		# this is a much less verbose version, for better readibiliy
		# but it does not show the intersection patterns so well
 		#return ("PropertyValue(value='%s',\n\tids=[ %s ])" % (self.value, \
 		#						self.devices))

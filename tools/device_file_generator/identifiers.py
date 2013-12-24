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
			# please deep copy this list
			self.ids = [DeviceIdentifier(dev, self.log) for dev in id.ids]
			return
			
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		if isinstance(id, list):
			# please deep copy this list
			self.ids = [DeviceIdentifier(dev, self.log) for dev in id]
			return
		
		self.ids = []
		if id != None:
			self.ids.append(DeviceIdentifier(id, self.log))
	
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
		other_ids = Identifiers(None, self.log)
		for id in ids:
			if id.string not in self.getAttribute('string'):
				other_ids.append(DeviceIdentifier(id))
		
		# our devices are equal to the input
		if (len(other_ids) == 0):
			return Identifiers(DeviceIdentifier(None, self.log), self.log)
		
		
		# create the intersection of all ids
		intersection_id = ids.intersection
		
		# strip the intersecting attributes from a copy of my own ids
		own_ids = Identifiers(self)
		for id in own_ids:
			for attr in id.properties:
				if id.properties[attr] == intersection_id.properties[attr]:
					setattr(id, attr, None)
		
		# if we only have one id we can stop here
		if len(own_ids) == 1:
			return Identifiers(own_ids[0], self.log)
		
		
		# strip the intersecting attributes from other_ids
		for id in other_ids:
			for attr in id.properties:
				if id.properties[attr] == intersection_id.properties[attr]:
					setattr(id, attr, None)
		
		own_union = own_ids.union
		# check which attributes are the same in my own ids
		same_attr = [ attr for attr in \
					[p for p in own_union.properties if own_union.properties[p] != None] \
					if '|' not in str(own_union.properties[attr]) ]
		
		# filter out these attributes from all the other ids
		if len(same_attr) and all(own_union.properties[attr] not in other_ids.getAttribute(attr) \
				for attr in same_attr):
			# there are no other ids that have the same common attributes
			# so we can forget the uncommon ones (like type)
			for attr in [p for p in own_union.properties if p not in same_attr]:
				setattr(own_union, attr, None)
			return Identifiers(own_union, self.log)
		
		
		# create sublists by filtering by name
		groups = [own_ids.filterForAttribute('name', name) for name in own_ids.getAttribute('name')]
		union_groups = [group.union for group in groups]
		
		# merge the ids in the list until we cannot anymore
		#devs = Identifiers(own_ids)
		devs = Identifiers(union_groups, self.log)
		unmergables = Identifiers()
		while(len(devs) > 0):
			current = devs[0]
			devs.remove(current)
			
			for dev in devs:
				union = current.unionWithDeviceIdentifier(dev)
				if all(id not in union for id in other_ids):
					devs.remove(dev)
					current = union
			
			self.log.debug("Unmergable: %s" % current)
			unmergables.append(current)
		
		# strip the unifying attributes from unmergables
		other_ids_union = ids.union
		for id in unmergables:
			for attr in id.properties:
				if id.properties[attr] == other_ids_union.properties[attr]:
					setattr(id, attr, None)
		
		return unmergables
	
	def filterForAttribute(self, name, value):
		ids = Identifiers()
		
		for id in self.ids:
			if value == id.properties[name]:
				ids.append(DeviceIdentifier(id, self.log))
		
		return ids
	
	def remove(self, id):
		self.ids.remove(id)
	
	def __contains__(self, id):
		return any(id in dev for dev in self.ids)
	
	def __iter__(self):
		for id in self.ids:
			yield id
	
	def __getitem__(self, index):
		return self.ids[index]
	
	def __len__(self):
		return len(self.ids)
	
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
		dev = DeviceIdentifier(self.ids[0], self.log)
		for id in self.ids[1:]:
			dev = dev.intersectionWithDeviceIdentifier(id)
		return dev
	
	@property
	def union(self):
		dev = DeviceIdentifier(logger=self.log)
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

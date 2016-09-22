# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

from logger import Logger
from device_identifier import DeviceIdentifier

class Identifiers:
	""" Identifiers
	Encapsulates a list of device identifiers.
	This manages filtering, merging and accessing of device ids.
	"""
	def __init__(self, device_id=None, logger=None):
		if isinstance(device_id, Identifiers):
			self.log = device_id.log
			# please deep copy this list
			self.ids = [DeviceIdentifier(dev, self.log) for dev in device_id.ids]
			return

		if logger == None:
			self.log = Logger()
		else:
			self.log = logger

		if isinstance(device_id, list):
			# please deep copy this list
			self.ids = [DeviceIdentifier(dev, self.log) for dev in device_id]
			return

		self.ids = []
		if device_id != None:
			self.ids.append(DeviceIdentifier(device_id, self.log))

	def append(self, device_id):
		assert isinstance(device_id, DeviceIdentifier)

		self.ids.append(device_id)
		self.ids = list(set(self.ids))
		self.ids.sort(key=lambda k : k.string)

	def extend(self, identifiers):
		assert isinstance(identifiers, Identifiers)

		for device_id in identifiers:
			self.ids.append(device_id)
		self.ids = list(set(self.ids))
		self.ids.sort(key=lambda k : k.string)

	def differenceFromIds(self, ids):
		assert isinstance(ids, Identifiers)

		# list all other ids that are not part of our ids
		other_ids = Identifiers(None, self.log)
		for device_id in ids:
			if device_id.string not in self.getAttribute('string'):
				other_ids.append(DeviceIdentifier(device_id))

		# our devices are equal to the input
		if (len(other_ids) == 0):
			return Identifiers(DeviceIdentifier(None, self.log), self.log)


		# create the intersection of all ids
		intersection_id = ids.intersection

		# strip the intersecting attributes from a copy of my own ids
		own_ids = Identifiers(self)
		for device_id in own_ids:
			for attr in device_id.properties:
				if device_id.properties[attr] == intersection_id.properties[attr]:
					setattr(device_id, attr, None)

		# if we only have one device_id we can stop here
		if len(own_ids) == 1:
			return Identifiers(own_ids[0], self.log)

		# strip the intersecting attributes from other_ids
		for device_id in other_ids:
			for attr in device_id.properties:
				if device_id.properties[attr] == intersection_id.properties[attr]:
					setattr(device_id, attr, None)

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


		# merge the ids in the list until we cannot anymore
		devs = Identifiers(own_ids)
		unmergables = Identifiers()
		while(len(devs) > 0):
			current = devs[0]

			if current in unmergables:
				devs.remove(current)
				continue

			remove_devs = [current]
			for dev in devs[1:]:
				if (dev in unmergables):
					remove_devs.append(dev)
					continue

				union = current.unionWithDeviceIdentifier(dev)
				if all(device_id not in union for device_id in other_ids):
					remove_devs.append(dev)
					current = union

			for dev in remove_devs:
				devs.remove(dev)

			# self.log.debug("\nUnmergable: %s" % current)
			unmergables.append(current)

		# strip the unifying attributes from unmergables
		other_ids_union = ids.union
		for device_id in unmergables:
			for attr in device_id.properties:
				if device_id.properties[attr] == other_ids_union.properties[attr]:
					setattr(device_id, attr, None)

		return unmergables

	def filterForAttribute(self, name, value):
		ids = Identifiers()

		for device_id in self.ids:
			if value == device_id.properties[name]:
				ids.append(DeviceIdentifier(device_id, self.log))

		return ids

	def remove(self, device_id):
		self.ids.remove(device_id)

	def __contains__(self, device_id):
		return any(device_id in dev for dev in self.ids)

	def __iter__(self):
		for device_id in self.ids:
			yield device_id

	def __getitem__(self, index):
		return self.ids[index]

	def __len__(self):
		return len(self.ids)

	def getAttribute(self, name):
		attributes = []

		for device_id in self.ids:
			attributes.append(getattr(device_id, name))

		if len(attributes) == 0:
			return [None]

		attributes = list(set(attributes))
		attributes.sort()
		return attributes


	@property
	def intersection(self):
		dev = DeviceIdentifier(self.ids[0], self.log)
		for device_id in self.ids[1:]:
			dev = dev.intersectionWithDeviceIdentifier(device_id)
		return dev

	@property
	def union(self):
		dev = DeviceIdentifier(logger=self.log)
		for device_id in self.ids:
			dev = dev.unionWithDeviceIdentifier(device_id)
		return dev

	@property
	def string(self):
		return ", ".join([str(device_id.string) for device_id in self.ids])

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return ("Identifiers( %s )" % self.string)

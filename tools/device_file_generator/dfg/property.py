# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

from logger import Logger

from .identifiers import Identifiers

class Property:
	""" Property
	Encapsulates a device property.
	This manages merging and accessing of properties.
	"""

	def __init__(self, device_id, name, value, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger

		self.name = name
		self.values = [PropertyValue(device_id, value, self.log)]

	def addValue(self, other):
		if isinstance(other.value, list):
			for prop in self.values:
				# create intersection of both lists
				intersection_value = [val for val in other.value if val in prop.value]
				# what are the differences in both lists?
				self_diff = [val for val in prop.value if val not in intersection_value]
				other_diff = [val for val in other.value if val not in intersection_value]

				# if there is an intersection, we can add the other.ids here
				if len(intersection_value) > 0:
					# if this value has more items than the intersection
					if len(self_diff) > 0:
						# set it to only the intersecting items
						prop.value = intersection_value
						# and add a new PropertyValue with only the differences
						self.values.append(PropertyValue(prop.ids, self_diff, self.log))
					# add the other.ids to this value
					prop.ids.extend(other.ids)
					# continue looking with the differences of other
					other.value = other_diff

				# order is important
				prop.value.sort()
				# no more values to add, we can stop looking now
				if len(other.value) == 0:
					return

		else:
			for prop in self.values:
				if (prop.value == other.value):
					prop.ids.extend(other.ids)
					return

		# apparently this value does not exist yet, so add it
		self.values.append(other)

	def getMergedProperty(self, other):
		assert isinstance(other, Property)
		assert other.name == self.name

		for value in other.values:
			self.addValue(value)

		self.values.sort(key=lambda k : k.value)

		return self

	def getValues(self):
		value_list = []
		for value in self.values:
			value_list.append(value.value)

		return value_list

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

	def __init__(self, device_id, value, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger

		if isinstance(device_id, list):
			self.ids = list(device_id)
		else:
			self.ids = Identifiers(device_id, self.log)
		self.value = value
		if isinstance(self.value, list):
			self.value.sort()

	@property
	def id(self):
		return self.ids.intersection

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return ("PropertyValue(value='%s',\nids= %s )" % (self.value, self.ids)).replace('\n', '\n\t')

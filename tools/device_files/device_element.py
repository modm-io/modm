#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Kevin Laeufer
# Copyright (c) 2013, Fabian Greif
# Copyright (c) 2013-2014, Niklas Hauser
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------
#
# This file contains the base class for all device elements
# i.e. Drivers and Properties
#

class DeviceElementBase:
	""" DeviceElementBase
	Base class for Property as well as Driver class.
	Handles pin-id/size-id and pin-count properties.
	"""

	def __init__(self, device, node):
		self.type = node.tag
		self.device = device # parent
		self.log = device.log

		# split multiple attribute values
		#self.attributes = {key : node.attrib[key].split('|') for key in node.attrib}
		self.attributes = {}
		for key in node.attrib:
			self.attributes[key] = node.attrib[key].split('|')

		# parse integer counts
		for key in self.attributes:
			if len(self.attributes[key]) == 1 and 'count' in key:
				self.attributes[key] = self._parseCount(self.attributes[key][0])

		# place 'none' with None
		for key in self.attributes:
			self.attributes[key] = [None if v == 'none' else v for v in self.attributes[key]]

	def _parseCount(self, count):
		if count.isdigit():
			return {'type': '==', 'count': int(count)}
		elif count[:-1].isdigit() and count[-1:] in ['+', '-']:
			return {'type': count[-1:], 'count': int(count[:-1])}
		return [count]

	def appliesTo(self, device_id, properties={}, matched=None):
		"""
		checks if this property/driver applies to the device specified by the
		device string
		"""
		if matched == None:
			matched = []

		for key in self.attributes:
			if 'device-' in key:
				# we need to compare this attribute to the device id
				dev_key = key.replace('device-', '').replace('-', '_')
				props = device_id.properties
				if dev_key in props:
					if props[dev_key] not in self.attributes[key]:
						return False
				else:
					self.log.error("DeviceElementBase: Attribute '%s' is not part of the DeviceIdentifier as '%s'."
								" Maybe you misspelled it?" % (key, dev_key))
					return False
			else:
				# these are the other attributes that we need to evauate
				props = []
				for prop in [p for p in properties if key == p.type]:
					# make sure we do not evaluate a circular reference
					hash = str(prop.type)+str(prop.value)
					if hash in matched[:-1]:
						self.log.error("DeviceElementBase: Cannot resolve circular references!"
									" '%s' depends on properties that reference this property." % prop.type)
						return False
					matched.append(hash)
					# these properties need to be evaluated recursively
					if prop.appliesTo(device_id, properties, matched):
						props.append(prop)

				for prop in props:
					attr = self.attributes[key]
					if '-count' in key:
						# this is a integer count
						if attr['type'] == '==':
							if attr['count'] != prop.value:
								return False
						elif attr['type'] == '+':
							if attr['count'] > prop.value:
								return False
						elif attr['type'] == '-':
							if attr['count'] < prop.value:
								return False
					else:
						if prop.value not in attr:
							return False
		return True

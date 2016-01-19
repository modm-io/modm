#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of the Roboterclub Aachen e.V. nor the
#    names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

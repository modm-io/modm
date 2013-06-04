#!/usr/bin/env python
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

import re

class DeviceElementBase:
	""" DeviceElementBase
	Base class for Property as well as Driver class.
	Handles pin-id/size-id and pin-count properties.
	"""

	def __init__(self, device, node):
		self.type = node.tag
		self.device = device # parent
		self.log = device.log
		# load attributes
		self.pin_id      = node.get('pin-id')
		self.size_id     = node.get('size-id')
		self.pin_count   = node.get('pin-count')
		self.device_platform = node.get('device-platform') # e.g. stm32
		self.device_family   = node.get('device-family')   # e.g. f4
		self.device_name     = node.get('device-name')     # e.g. 405
		self.device_type     = node.get('device-type')     # e.g. for avr: m1
		# Split Attributes that can be split
		if self.pin_id != None:
			self.pin_id = self.pin_id.split('|')
		if self.size_id != None:
			self.size_id = self.size_id.split('|')
		if self.device_platform != None:
			self.device_platform = self.device_platform.split('|')
		if self.device_family != None:
			self.device_family = self.device_family.split('|')
		if self.device_name != None:
			self.device_name = self.device_name.split('|')
		if self.device_type != None:
			self.device_type = self.device_type.split('|')
		# parse pin count
		[self.pin_count, self.pin_count_type] = self._parsePinCount(self.pin_count)

	def _parsePinCount(self, pin_count):
		if pin_count == None:
			return [None, None]
		if pin_count.isdigit():
			pin_count_type = '=='
			pin_count = int(pin_count)
		elif pin_count[:-1].isdigit() and pin_count[-1:] in ['+', '-']:
			pin_count_type = pin_count[-1:]
			pin_count = int(pin_count[:-1])
		elif pin_count:
			raise ParserException("Pincount needs to be an integer value that can be followed by +/-: '%s' is not a valid pincount." % (pin_count))
		return [pin_count, pin_count_type]

	def appliesTo(self, device_string, pin_count=10000):
		"""
		checks if this property/driver applies to the device specified by the
		device string
		"""
		s = device_string
		if self.device_platform != None:
			if s.platform == None or s.platform not in self.device_platform:
				return False
		if self.device_family != None:
			if s.family == None or s.family not in self.device_family:
				return False
		if self.device_name != None:
			if s.name == None or s.name not in self.device_name:
				return False
		if self.device_type != None:
			if s.type == None or s.type not in self.device_type:
				return False
		if self.pin_id != None:
			if s.pin_id == None or s.pin_id not in self.pin_id:
				return False
		if self.size_id != None:
			if s.size_id == None or s.size_id not in self.size_id:
				return False
		if self.pin_count != None and self.pin_count > 0:
			if self.pin_count_type == '==':
				if self.pin_count != pin_count:
					return False
			elif self.pin_count_type == '+':
				if self.pin_count > pin_count:
					return False
			elif self.pin_count_type == '-':
				if self.pin_count < pin_count:
					return False
		return True

################## Helper Classes #############################################
class DeviceString:
	""" DeviceString
	Helper class to parse a device string, e.g. "stm32f407vg".
	TODO: Add parsing capabilities for atmel (i.e. avr, xmega, avr32) as
	well as lpc controllers
	"""

	def __init__(self, string=None):
		self.string = string
		# default for properties is None
		self.platform = None # e.g. stm32, avr
		self.family   = None # e.g.    f4, atmega
		self.name     = None # e.g.   405, 16
		self.type     = None # e.g. -----, m1
		self.pin_id   = None # e.g.     r, -----
		self.size_id  = None # e.g.     g, 16
		self.valid = False
		
		if string == None:
			return
		
		# try to determine platform and to parse string accordingly
		if string.startswith('stm32f'):
			self.platform = "stm32"
			self.family = string[5:7]
			self.name = string[6:9]
			if len(string) >= 10:
				self.pin_id = string[9]
			if len(string) >= 11:
				self.size_id = string[10]
			if len(string) >= 9:
				self.valid = True

		# AVR platform with ATmega and ATxmega family
		elif string.startswith('at'):
			self.platform = "avr"
			match = re.search("(?P<family>attiny|atmega|atxmega)(?P<name>\d+)", string)
			if match:
				self.family = match.group('family')
				self.name = match.group('name')

				if self.family == "atmega" or self.family == "attiny":
					match = re.search(self.family + self.name + "(?P<type>\w*)-?(?P<package>\w*)", string)
					if match.group('type') != '':
						self.type = match.group('type').lower()
					if match.group('package') != '':
						self.pin_id = match.group('package').lower()
					match = re.search("(?P<size>4|8|16|32|64|128|256)\d*", self.name)
					if match:
						self.size_id = match.group('size')

				elif self.family == "atxmega":
					match = re.search(self.family + self.name + "(?P<type>[A-Ea-e])(?P<package>[1-4])(?P<usb>[Bb][Uu])", string)
					if match.group('type') != '':
						self.type = match.group('type').lower()
					if match.group('package') != '':
						self.pin_id = match.group('package').lower()
				self.valid = True
			
		else:
			raise ParserException("Parse Error: unknown platform. Device string: %" % (string))

	def getTargetDict(self):
		dict = {'target': {}}
		dict['target']['platform'] = self.platform
		dict['target']['family'] = self.family
		dict['target']['name'] = self.name
		dict['target']['type'] = self.type
		dict['target']['pin_id'] = self.pin_id
		dict['target']['size_id'] = self.size_id
		dict['target']['string'] = self.string
		return dict
	
	def getComparisonDict(self, other):
		if not isinstance(other, DeviceString):
			return None
		tself = self.getTargetDict()['target']
		tother = other.getTargetDict()['target']
		dict = {'common_keys': [], 'different_keys': []}
		device = DeviceString()
		device_delta = DeviceString()
		
		for key in tself:
			if tself[key] == tother[key]:
				dict['common_keys'].append(key)
				setattr(device, key, tself[key])
			else:
				dict['different_keys'].append(key)
				if tself[key] == None:
					setattr(device_delta, key, tother[key])
				if tother[key] == None:
					setattr(device_delta, key, tself[key])
		
		common = ""
		if 'platform' in dict['common_keys']:
			if self.platform != "avr":
				common += self.platform
		if 'family' in dict['common_keys']:
			common += self.family
		if 'name' in dict['common_keys']:
			common += self.name
		if 'type' in dict['common_keys'] and self.type != None:
			common += self.type
		device.string = common;
		dict['device'] = device
		dict['device_delta'] = device_delta
		return dict

	def __repr__(self):
		return "DeviceString(" + self.__str__() + ")"

	def __str__(self):
		target = self.getTargetDict()['target']
		target = {o:target[o] for o in target if target[o] != None}
		return str(target)

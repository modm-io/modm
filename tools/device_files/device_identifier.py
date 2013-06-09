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

import re

class DeviceIdentifier:
	""" DeviceIdentifier
	A class to parse a device string, e.g. "stm32f407vg".
	TODO: Add parsing capabilities for atmel (i.e. avr, xmega, avr32) as
	well as lpc controllers
	"""

	def __init__(self, string=None):
		self._string = string
		# default for properties is None
		self.platform = None # e.g. stm32, avr
		self.family   = None # e.g.	f4, atmega
		self.name	 = None # e.g.   405, 16
		self.type	 = None # e.g. -----, m1
		self.pin_id   = None # e.g.	 r, -----
		self.size_id  = None # e.g.	 g, 16
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
					match = re.search("(?P<size>2|4|8|16|32|64|128|256)\d*", self.name)
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

	@property
	def string(self):
		string = ""
		if self.platform != None and self.platform != "avr":
			string += platform
		if self.family != None:
			string += self.family
		if self.name != None:
			string += self.name
		if self.type != None and self.platform != "stm32":
			string += self.type
		if self.pin_id != None and self.platform != "avr":
			string += self.pin_id
		if self.size_id != None and self.platform != "avr":
			string += self.size_id
		return string

	@property
	def properties(self):
		dict = {}
		dict['platform'] = self.platform
		dict['family'] = self.family
		dict['name'] = self.name
		dict['type'] = self.type
		dict['pin_id'] = self.pin_id
		dict['size_id'] = self.size_id
		return dict
	
	def getTargetDict(self):
		return {'target': self.properties}
	
	def isEmpty(self):
		empty = True
		target = self.properties
		for key in target:
			if target[key] != None:
				empty = False
		return empty
	
	def getComparisonDict(self, other):
		"""
		This method compares its own properties with the other class and
		generates three new Devices: a common Device with the common
		properties of both, and two children devices _only_ with the differences.
		Additionally, a common string in generated. 
		"""
		if not isinstance(other, DeviceIdentifier):
			return NotImplemented
		
		tself = self.properties
		tother = other.properties
		dict = {'common_keys': [], 'different_keys': []}
		
		common = DeviceIdentifier()
		self_delta = DeviceIdentifier()
		other_delta = DeviceIdentifier()
		
		for key in tself:
			if tself[key] == tother[key]:
				dict['common_keys'].append(key)
				setattr(common, key, tself[key])
			else:
				dict['different_keys'].append(key)
				setattr(other_delta, key, tother[key])
				setattr(self_delta, key, tself[key])
				self_delta.valid = True
				other_delta.valid = True
		
		common.valid = True
		
		dict['common'] = common
		dict['self_delta'] = self_delta
		dict['other_delta'] = other_delta
		return dict

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		target = self.properties
		target = {o:target[o] for o in target if target[o] != None}
		return "DeviceIdentifier(" + str(target) + ")"

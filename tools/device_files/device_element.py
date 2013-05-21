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

class DeviceElementBase:
	""" DeviceElementBase
	Base class for Property as well as Driver class.
	Handles pin-id/size-id and pin-count properties.
	"""

	def __init__(self, device, node):
		self.node = node
		self.type = node.tag
		self.device = device # parent
		# load attributes
		self.pin_id    = node.get('pin-id')
		self.size_id   = node.get('size-id')
		self.pin_count = node.get('pin-count')
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
		if self.pin_id != None:
			if s.pin_id == None or self.pin_id != s.pin_id:
				return False
		if self.size_id != None:
			if s.size_id == None or self.size_id != s.size_id:
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

	def __init__(self, string):
		self.string = string
		# default for properties is None
		self.platform = None
		self.family   = None
		self.name     = None
		self.pin_id   = None
		self.size_id  = None
		self.valid = False
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
		else:
			raise ParserException("Parse Error: unknown platform. Device string: %" % (string))

	def getTargetDict(self):
		dic = {'target': {}}
		dic['target']['platform'] = self.platform
		dic['target']['family'] = self.family
		dic['target']['name'] = self.name
		dic['target']['pin_id'] = self.pin_id
		dic['target']['size_id'] = self.size_id
		dic['target']['string'] = self.string
		return dic

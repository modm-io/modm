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
		self.device_core     = node.get('device-core')
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
		if self.device_core != None:
			self.device_core = self.device_core.split('|')
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

	def appliesTo(self, device_string, pin_count=10000, core=None):
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
		if self.device_core != None:
			if core == None or len(core) == 0 or core not in self.device_core:
				return False
		return True

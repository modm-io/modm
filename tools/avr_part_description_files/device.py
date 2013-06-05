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

import re, os, sys
from string import Template
from parser import XMLDeviceParser

# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger
# add python module device files to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))
from device_element import DeviceString

class AVRDevice:
	""" AVRDevice
	Represents a device.
	"""

	def __init__(self, description_file=None, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		if description_file == None:
			self.properties = {}
			self.properties['similar'] = []
			return
		
		if isinstance(description_file, PartDescriptionFile):
			self.properties = dict(description_file.properties)
		else:
			self.properties = dict(description_file)
		self.properties['similar'] = []

		# if flash or ram is missing, it is a bad thing and unsupported
		if 'flash' not in self.properties:
			self.log.error("No FLASH found")
			return None
		if 'ram' not in self.properties:
			self.log.error("No RAM found")
			self.log.error("XPCC does not support Assembler-only programming!")
			return None
		# eeprom is optional on AVR and not available on ARM devices
		if 'eeprom' not in self.properties and 'avr' == self.properties['device'].platform:
			self.log.warn("No EEPROM found")


	def getComparisonDict(self, other):
		current_dict, past_dict = self.properties, other.properties
		current_keys, past_keys = [
				set(d.keys()) for d in (current_dict, past_dict)
			]
		intersect = current_keys.intersection(past_keys)

		changed = set()
		for o in intersect:
			if isinstance(past_dict[o], list):
				same = True
				for item in past_dict[o]:
					if item not in current_dict[o]:
						same = False
				if same == False:
					changed.add(o)
			else:
				if past_dict[o] != current_dict[o]:
					changed.add(o)
		unchanged = intersect - changed
		return {'changed': changed, 'unchanged': unchanged}


	def getMergedDevice(self, other):
		"""
		Merges the values of both devices and add a dictionary of differences
		"""
		self.log.info("Merging " + self.properties['device'].string + " and " + other.properties['device'].string)

		# calculate the difference
		diff = self.getComparisonDict(other)
		# they are the same device
		if len(diff['changed']) == 0:
			return self

		comparison = self.properties['device'].getComparisonDict(other.properties['device'])
		self.log.debug("'device' differs: " + str(comparison['different_keys']) + " " + comparison['device'].string)
		
		# build a new AVRDevice
		device = AVRDevice(None, self.log)
		device.properties['device'] = comparison['device']
		delta = AVRDevice(None, self.log)
		delta.properties['device'] = comparison['device_delta']
		
		for key in diff['unchanged']:
			device.properties[key] = self.properties[key]
		
		for key in diff['changed']:
			value1 = self.properties[key]
			value2 = other.properties[key]
			if key != 'device':
				self.log.debug("'" + key + "' differs:\n" + str(value1) + "\n" + str(value2))
				if isinstance(value1, list):
					common = list(set(value1).intersection(value2))
					oneMtwo = list(set(value1).difference(value2))
					twoMone = list(set(value2).difference(value1))
					# add the common to the device
					device.properties[key] = common
					delta.properties[key] = twoMone
				else:
					device.properties[key] = value1
					delta.properties[key] = value2
		
		device.properties['similar'].append(delta)
		
		return device

	def __repr__(self):
		return "AVRDevice(" + self.__str__() + ")"

	def __str__(self):
		s = "["
		for key in self.properties:
			s += "{'" + key + "': '" + str(self.properties[key]) + "'}, \n"
		return s + "]"

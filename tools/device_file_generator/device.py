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
from device_string import DeviceString

class Device:
	""" Device
	Represents a device.
	"""

	def __init__(self, description_file=None, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		if description_file == None:
			self.properties = {}
			self.properties['instances'] = []
			return
		
		# proper handling of Parsers
		if isinstance(description_file, XMLDeviceParser):
			self.properties = dict(description_file.properties)
		else:
			self.properties = dict(description_file)
		self.properties['instances'] = []

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
		own_keys, other_keys = [
				set(d.keys()) for d in (self.properties, other.properties)]
		intersect = own_keys.intersection(other_keys)

		changed = set()
		for o in intersect:
			if isinstance(other.properties[o], list):
				same = True
				for item in other.properties[o]:
					if item not in self.properties[o]:
						same = False
				if same == False:
					changed.add(o)
			else:
				if other.properties[o] != self.properties[o]:
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
		
		# build a new parent Device, with all the common features
		parent = Device(None, self.log)
		parent.properties['device'] = comparison['device']
		
		# build a new child Device, with only the delta information
		child1 = Device(None, self.log)
		child1.properties['device'] = comparison['device_delta']
		
		for key in diff['unchanged']:
			parent.properties[key] = self.properties[key]
		
		for key in diff['changed']:
			value1 = self.properties[key]
			value2 = other.properties[key]
			if key != 'device':
				self.log.debug("'" + key + "' differs:\n" + str(value1) + "\n" + str(value2))
				if isinstance(value1, list):
					common = list(set(value1).intersection(value2))
					oneMtwo = list(set(value1).difference(value2))
					twoMone = list(set(value2).difference(value1))
					# add the common to the parent
					parent.properties[key] = common
					child1.properties[key] = twoMone
				else:
					parent.properties[key] = value1
					child1.properties[key] = value2
		
		parent.properties['instances'].append(child1)
		
		return parent

	def __repr__(self):
		return "Device(" + self.__str__() + ")"

	def __str__(self):
		s = "["
		for key in self.properties:
			s += "{'" + key + "': '" + str(self.properties[key]) + "'}, \n"
		return s + "]"

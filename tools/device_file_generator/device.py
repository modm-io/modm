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


from reader import XMLDeviceReader

import os, sys
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
			self.properties = {'instances': []}
			return
		
		# proper handling of Parsers
		if isinstance(description_file, XMLDeviceReader):
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
		self_keys = set(self.properties.keys())
		other_keys = set(other.properties.keys())
		intersect = self_keys.intersection(other_keys)

		changed = set()
		unchanged = set()
		for o in intersect:
			if isinstance(other.properties[o], list):
				same = True
				for item in other.properties[o]:
					if item not in self.properties[o]:
						same = False
				for item in self.properties[o]:
					if item not in other.properties[o]:
						same = False
				if same == False:
					changed.add(o)
			else:
				if other.properties[o] != self.properties[o]:
					changed.add(o)
		
		unchanged = intersect - changed
		self_only = self_keys - intersect
		other_only = other_keys - intersect
		return {'changed': changed, 'unchanged': unchanged, 'self-only': self_only, 'other-only': other_only}


	def getMergedDevice(self, other):
		"""
		Merges the values of both devices and add a dictionary of differences
		"""
		self.log.info("Merging " + self.properties['device'].string + " and " + other.properties['device'].string)

		# calculate the difference
		diff = self.getComparisonDict(other)
		
		self.log.debug("Changed: " + str(diff['changed']))
		self.log.debug("Unchanged: " + str(diff['unchanged']))
		self.log.debug("Self-only: " + str(diff['self-only']))
		self.log.debug("Other-only: " + str(diff['other-only']))
		
		# they are the same device
		if len(diff['changed']) == 0:
			self.log.warn("Some device!")
			return self

		comparison = self.properties['device'].getComparisonDict(other.properties['device'])
		self.log.debug("'device' differs: " + str(comparison['different_keys']) + " " + comparison['common'].string)
		
		# build a new parent Device, with all the common features
		parent = Device(None, self.log)
		parent.properties['device'] = comparison['common']
		
		# build two new child Devices, with only the delta information
		self_child = Device(None, self.log)
		self_child.properties['device'] = comparison['self_delta']
		other_child = Device(None, self.log)
		other_child.properties['device'] = comparison['other_delta']
		
		# unchanged properties obviously belong into the parent
		for key in diff['unchanged']:
			parent.properties[key] = self.properties[key]
		
		# changed properties must be handled specially
		for key in diff['changed']:
			self_value = self.properties[key]
			other_value = other.properties[key]
			if key != 'device':
				self.log.debug("'" + key + "' differs:\n" + str(self_value) + "\n" + str(other_value))
				# for lists we want the difference sets in the children devices
				# and the common set in the parent device
				# This code assumes, that both devices have the same dictionary structure!
				if isinstance(self_value, list):
					# if this is a list of dictionaries, we need to manually compare them
					if len(self_value) > 0 and isinstance(self_value[0], dict) or \
						len(other_value) > 0 and isinstance(other_value[0], dict):
						common = []
						self_minus_other = []
						other_minus_self = []
						for sval in self_value:
							if sval in other_value:
								common.append(sval)
								other_value.remove(sval)
							else:
								self_minus_other.append(sval)
							self_value.remove(sval)
						other_minus_self = list(other_value)
					else:
						common = list(set(self_value).intersection(other_value))
						self_minus_other = list(set(self_value).difference(other_value))
						other_minus_self = list(set(other_value).difference(self_value))
						
					# add the common to the parent
					if len(common) > 0:
						parent.properties[key] = common
					# and the differences to the children
					if len(self_minus_other) > 0:
						self_child.properties[key] = self_minus_other
					if len(other_minus_self) > 0:
						other_child.properties[key] = other_minus_self
				else:
					self_child.properties[key] = self_value
					other_child.properties[key] = other_value
		
		# self/other-only keys must be added to the children
		for key in diff['self-only']:
			self_child.properties[key] = self.properties[key]
		for key in diff['other-only']:
			other_child.properties[key] = other.properties[key]
		
		parent.addInstance(self_child)
		parent.addInstance(other_child)
		
		return parent
	
	def addInstance(self, device):
		if device.properties['device'].isEmpty() and len(device.properties) <= 2:
			# do not add an empty device
			if len(device.properties['instances']) == 0:
				return
			
			self.properties['instances'].extend(device.properties['instances'])
		else:
			self.properties['instances'].append(device)
		

	def getAttributeRecursive(self, attr):
		# the device can already be merged!
		# so we need to take the instances into account
		# we assume that every instance is in the same category
		attribute = getattr(self.properties['device'], attr)
		if attribute == None and len(self.properties['instances']) > 0:
			attribute = getattr(self.properties['instances'][0].properties['device'], attr)
		return attribute
	
	def getNameArray(self):
		names = []
		if self.properties['device'].name != None:
			names.append(self.properties['device'].name)
		else:
			for inst in self.properties['instances']:
				names.extend(inst.getNameArray())
		return names

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		s = "\n Device("
		length = len(self.properties)
		i = 0
		for key in self.properties:
			if key == 'instances':
				ilen = len(self.properties['instances'])
				ii = 0
				string = "{'instances': '["
				for dev in self.properties['instances']:
					string += str(dev)
					if ii < ilen-1:
						string +=  "', '"
					ii += 1
				s += string.replace("\n", "\n\t") + "]'}"
			else:
				s += "{'" + key + "': '" + str(self.properties[key]) + "'}"
			if i < length-1:
				s += ",\n   "
			i += 1
		return s + ")"

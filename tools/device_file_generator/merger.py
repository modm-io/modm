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

import sys, os
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

class DeviceMerger:
	""" DeviceMerger
	Merges several devices into logical groups.
	Since most Devices have the same peripherals, but differ in other
	parameters, like Flash/RAM/EEPROM sizes, packaging or other minor
	differences, it makes sense to group them accordingly.
	
	Please not that this class only makes the resulting XML files more user
	friendly to manually edit, since the user does not have to apply the
	changes to multiple files. Idealy, of course reality might differ :(.
	"""

	def __init__(self, devices, logger=None):
		# copy this list, since we will delete items
		self.devices = list(devices)
		self.mergedDevices = []

		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		mergedByType = self._mergeDevicesByType(self.devices)
		mergedByName = self._mergeDevicesByName(mergedByType)
		self.mergedDevices = mergedByName

	
	def _mergeDevicesByName(self, devices):
		"""
		This is a simple helper method to merge devices based on name.
		"""
		avrDevices = []
		result = []
		
		for dev in devices:
			if dev.properties['device'].platform == 'avr':
				avrDevices.append(dev)
			else:
				result.append(dev)
		
		avrDevices = self._mergeDevicesByNameAVR(avrDevices)
		result.extend(avrDevices)
		
		return result
	
	def _mergeDevicesByNameAVR(self, devices):
		"""
		This checks the size-id and name of the devices, and merges the devices
		based on the observation, that the size-id only influences the size of
		memories, i.e. FLASH, RAM, and EEPROM.
		"""
		# copy the devices, since this array will be modified
		devs = list(devices)
		merged = []
		
		while len(devs) > 0:
			current = devs[0]
			devs.remove(current)
			
			matches = []
			size_id = current.getAttributeRecursive('size_id')
			
			if size_id != None:
				name = current.getAttributeRecursive('name')
				type = current.getAttributeRecursive('type')
				family = name[len(size_id):]
				
				if not (family == "" and type == None):
					type = self._getCategoryTypeAVR(current)
					
					string = "ByName: Searching for device ending in '" + family + "' and '" + str(type)
					self.log.info(string + "'")
					
					for dev in devs:
						dname = dev.getAttributeRecursive('name')
						dsize_id = dev.getAttributeRecursive('size_id')
						
						# if they do not have a size-id they are probably unmergable
						if dsize_id == None:
							continue
						dfamily = dname[len(dsize_id):]
						
						# perpare for type comparison
						# we should only merge when the family is the same,
						# and if the type is the same
						
						if dfamily == family and dev.getAttributeRecursive('type') in type:
							matches.append(dev)
				
				for match in matches:
					devs.remove(match)
					current = current.getMergedDevice(match)
			
			if len(matches) == 0:
				self.log.info("ByName: no match for device: " + current.properties['device'].string)
			
			self.log.debug("ByName:\n\nResulting device: " + str(current))
			merged.append(current)
		
		return merged
	
	
	def _mergeDevicesByType(self, devices):
		"""
		This is a simple helper method to merge devices based on type.
		"""
		avrDevices = []
		result = []
		
		for dev in devices:
			if dev.properties['device'].platform == 'avr':
				avrDevices.append(dev)
			else:
				result.append(dev)
			
		avrDevices = self._mergeDevicesByTypeAVR(avrDevices)
		result.extend(avrDevices)
		
		return result
	
	def _mergeDevicesByTypeAVR(self, devices):
		"""
		This checks the name suffix (for example 'P', 'A', 'PA') of the
		devices and merges them based on the observation, that the suffix
		does not have anything to do with the mapping of peripherals.
		"""
		devs = list(devices)
		merged = []
		
		while len(devs) > 0:
			current = devs[0]
			devs.remove(current)
			
			props = current.properties['device']
			if props.valid == False:
				continue
			
			matches = []
			suffix = self._getCategoryTypeAVR(current)
			
			self.log.info("ByType: Searching for device ending in " + str(suffix)) 
			
			for dev in devs:
				if dev.properties['device'].name == props.name:
					if dev.properties['device'].type in suffix:
						matches.append(dev)
			
			for match in matches:
				devs.remove(match)
				current = current.getMergedDevice(match)
			
			if len(matches) == 0:
				self.log.info("ByType: No match for device: " + current.properties['device'].string)
			
			self.log.debug("ByType:\n\nResulting device: " + str(current))
			merged.append(current)
		
		return merged


	def _getCategoryTypeAVR(self, device):
		type = device.getAttributeRecursive('type')
		# these are the categories of mergable types
		cat1 = [None, 'p', 'a', 'pa']
		cat2 = ['rfa1', 'rfa2', 'rfr1', 'rfr2']
		cat3 = ['hva', 'hvb', 'hve2', 'hvbrevb']
		cat4 = ['u2', 'u4', 'u6']
		cat5 = ['m1', 'c1']
		# make sure that only one category is used!
		if type in cat2:
			cat = cat2
		elif type in cat3:
			cat = cat3
		elif type in cat4:
			cat = cat4
		elif type in cat5:
			cat = cat5
		else:
			cat = cat1
		return cat

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
import xml.etree.ElementTree as et
import xml.parsers.expat
from string import Template
from parser_exception import ParserException

# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger
# add python module device files to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))
from device_element import DeviceString

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
		self.merged_devices = []

		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		merged = self._mergeDevicesByName(self.devices)
		self.merged_devices = merged


	def _mergeDevicesByName(self, devices):
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
			
			s = current.properties['device']
			if s.valid == False:
				merged.append(current)
				continue

			matches = []
			suffix = [None, 'p', 'a', 'pa']
			for dev in devs:
				if dev.properties['device'].name == current.properties['device'].name:
					if dev.properties['device'].type in suffix:
						matches.append(dev)
			
			for match in matches:
				devs.remove(match)
				current = current.getMergedDevice(match)
			
			merged.append(current)
			self.log.debug("Resulting device: " + str(current))

		return merged
	
#	def __str__(self):
#		return ""


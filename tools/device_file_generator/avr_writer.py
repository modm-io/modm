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

from writer import XMLDeviceWriter, XMLElement

import os, sys
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

class AVRDeviceWriter(XMLDeviceWriter):
	""" AVRPartDescriptionFile
	Represents a device in xml format.
	"""

	def __init__(self, device, logger=None):
		XMLDeviceWriter.__init__(self, device, logger)
		
		self.types = self.device.getDeviceTypes()
		
		self.root.setAttribute('type', "|".join(self.types))
		
		self.addDeviceAttributesToNode(self.root, 'flash')
		self.addDeviceAttributesToNode(self.root, 'ram')
		self.addDeviceAttributesToNode(self.root, 'eeprom')
		self.addDeviceAttributesToNode(self.root, 'mmcu')
		self.addDeviceAttributesToNode(self.root, 'core')
		
		self.addDeviceAttributesToNode(self.root, 'define')
		
		# AVR specific
		child = self.root.addChild('pin-count')
		child.setValue('-1')
		
		for header in ['avr/io.h', 'avr/interrupt.h']:
			child = self.root.addChild('header')
			child.setValue(header)
		
		# drivers
		self.addGpioToNode(self.root)
		

	def addDeviceAttributesToNode(self, node, name):
		list = self.device.getAttributes(name)
		for item in list:
			child = node.addChild(name)
			target = item['id'].properties
			dict = {}
			for attr in target:
				if target[attr] != None:
					if attr == 'type' and len(self.types) > 1:
						dict[attr] = target[attr]
					if attr == 'name' and len(self.names) > 1:
						dict[attr] = target[attr]
			child.setAttributes(dict)
			child.setValue(item['value'])
	
	def addGpioToNode(self, node):
		list = self.device.getAttributes('gpios')
		driver = node.addChild('driver')
		driver.setAttributes({'type': 'gpio', 'name': self.family})
		for item in list:
			target = item['id'].properties
			dict = {}
			for attr in target:
				if attr in ['type', 'name'] and target[attr] != None:
					dict[attr] = target[attr]
			ports = item['value']
			ports.sort(key=lambda k: k['port'])
			for port in item['value']:
				gpios = self._getAttributedPortDictionary(port)
				for gpio in gpios:
					child = driver.addChild('gpio')
					#child.setAttributes(dict)
					child.setAttributes(gpio)

	def _getAttributedPortDictionary(self, port):
		ports = []
		mask = port['mask']
		id = 0
		
		while id < 8:
			if mask & 0x01:
				ports.append({'port': port['port'], 'id': str(id)})
			mask >>= 1
			id += 1
		
		return ports
	
	def write(self, folder):
		names = self.device.getDeviceNames()
		names.sort(key=int)
		types = self.device.getDeviceTypes()
		name = self.device.id.family + "-".join(["_".join(names), "_".join(types)]) + ".xml"
		self.writeToFolder(folder, name)

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "AVRDeviceWriter(\n" + self.toString() + ")"

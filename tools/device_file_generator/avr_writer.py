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
	""" AVRDeviceWriter
	Translates the Device to a XPCC specific format.
	"""
	def __init__(self, device, logger=None):
		XMLDeviceWriter.__init__(self, device, logger)
		
		self.types = self.device.getDeviceTypes()
		
		self.root.setAttribute('type', "|".join(self.types))
		
		self.addDeviceAttributesToNode(self.root, 'flash')
		self.addDeviceAttributesToNode(self.root, 'ram')
		self.addDeviceAttributesToNode(self.root, 'eeprom')
		self.addDeviceAttributesToNode(self.root, 'mmcu')
		self.addDeviceAttributesToNode(self.root, 'define')
		
		self.addDeviceAttributesToNode(self.root, 'core')
		
		pin_count_child = self.root.addChild('pin-count')
		pin_count_child.setValue(0)
		
		core_child = self.root.addChild('driver')
		core_child.setAttributes({'type': 'core', 'name': 'avr'})
		ram_sizes = self.device.getAttributes('ram')
		ram_sizes.sort(key=lambda k : int(k['id'].name or 0))
		for ram_size in ram_sizes:
			size = ram_size['value']
			# for large RAM sizes, reserve 1kB for stack
			# for small RAM sizes, reserve half of entire size for stack
			if size > 2048:
				size -= 1024
			else:
				size /= 2
			ram_size_child = core_child.addChild('parameter')
			ram_size_child.setAttributes({'device-name': ram_size['id'].name, 'name': 'ram_length'})
			ram_size_child.setValue(size)
		
		for header in ['avr/io.h', 'avr/interrupt.h', 'avr/sleep.h']:
			header_child = self.root.addChild('header')
			header_child.setValue(header)

		# GPIO
		self.addGpioToNode(self.root)
		# UART
		self.addUartToNode(self.root)
		# I2C aka TWI
		self.addI2cToNode(self.root)
		# SPI
		self.addSpiToNode(self.root)
		# ADC
		self.addAdcToNode(self.root)
		

	def addDeviceAttributesToNode(self, node, name):
		attributes = self.device.getAttributes(name)
		attributes.sort(key=lambda k : (int(k['id'].name or 0), k['id'].type))
		for item in attributes:
			child = node.addChild(name)
			child.setAttributes(self._getAttributeDictionaryFromId(item['id']))
			child.setValue(item['value'])
	
	def addModuleAttributesToNode(self, node, peripheral, name):
		attributes = self._getModuleAttributes()
		for item in attributes:
			dict = self._getAttributeDictionaryFromId(item['id'])
			for module in item['value']:
				if peripheral in module:
					driver = node.addChild('driver')
					driver.setAttributes(dict)
					driver.setAttributes({'type': name, 'name': self.family})
	
	def addI2cToNode(self, node):
		self.addModuleAttributesToNode(node, 'TWI', 'i2c')
	
	def addSpiToNode(self, node):
		self.addModuleAttributesToNode(node, 'SPI', 'spi')
	
	def addAdcToNode(self, node):
		self.addModuleAttributesToNode(node, 'AD_CONVERTER', 'adc')
	
	def addUartToNode(self, node):
		attributes = self._getModuleAttributes()
		
		for item in attributes:
			instances = []
			dict = self._getAttributeDictionaryFromId(item['id'])
			for module in item['value']:
				if 'USART' in module and 'SPI' not in module:
					# some device only have a 'USART', but we want 'USART0'
					mod = module + '0'
					instances.append(mod[5:6])
			if instances != []:
				driver = node.addChild('driver')
				driver.setAttributes(dict)
				driver.setAttributes({'type': 'uart', 'name': self.family, 'instances': ",".join(instances)})
	
	def _getModuleAttributes(self):
		attributes = self.device.getAttributes('modules')
		newAttributes = attributes
		return newAttributes
	
	def addGpioToNode(self, node):
		attributes = self.device.getAttributes('gpios')
		driver = node.addChild('driver')
		driver.setAttributes({'type': 'gpio', 'name': self.family})
		for item in attributes:
			gpios = item['value']
			gpios.sort(key=lambda k: k['port'])
			dict = self._getAttributeDictionaryFromId(item['id'])
			for gpio in gpios:
				gpio_child = driver.addChild('gpio')
				gpio_child.setAttributes(dict)
				gpio_child.setAttributes({'port': gpio['port'], 'id': gpio['id']})
				if 'pcint' in gpio:
					pcint_child = gpio_child.addChild('af')
					pcint_child.setAttributes({'type': 'pcint', 'id': gpio['pcint']})
				if 'extint' in gpio:
					extint_child = gpio_child.addChild('af')
					extint_child.setAttributes({'type': 'extint', 'id': gpio['extint']})

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
	
	def _getAttributeDictionaryFromId(self, id):
		target = id.properties
		dict = {}
		for attr in target:
			if target[attr] != None:
				if attr == 'type' and len(self.types) > 1:
					dict['device-type'] = target[attr]
				if attr == 'name' and len(self.names) > 1:
					dict['device-name'] = target[attr]
		return dict
	
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

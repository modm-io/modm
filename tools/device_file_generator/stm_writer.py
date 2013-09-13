# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

from writer import XMLDeviceWriter, XMLElement

import os, sys
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

class STMDeviceWriter(XMLDeviceWriter):
	""" STMDeviceWriter
	Translates the Device to a XPCC specific format.
	"""
	def __init__(self, device, logger=None):
		XMLDeviceWriter.__init__(self, device, logger)
		
		self.addDeviceAttributesToNode(self.root, 'flash')
		self.addDeviceAttributesToNode(self.root, 'ram')
		self.addDeviceAttributesToNode(self.root, 'define')
		
		self.addDeviceAttributesToNode(self.root, 'core')
		
		self.addDeviceAttributesToNode(self.root, 'pin-count')
		
		core_child = self.root.addChild('driver')
		core_child.setAttributes({'type': 'core', 'name': 'cortex'})
		
		header_child = self.root.addChild('header')
		header_child.setValue('stm32%sxx.h' % self.device.id.family)

		# UART
		self.addUartToNode(self.root)
		# I2C aka TWI
		self.addI2cToNode(self.root)
		# SPI
		self.addSpiToNode(self.root)
		# ADC
		self.addAdcToNode(self.root)
		# DAC
		self.addDacToNode(self.root)
		# Timer
		self.addTimerToNode(self.root)
		# GPIO
		self.addGpioToNode(self.root)
		

	def addDeviceAttributesToNode(self, node, name):
		attributes = self.device.getAttributes(name)
		if name == 'flash':
			seen = set()
			attributes = [a for a in attributes if a['value'] not in seen and not seen.add(a['value'])]
		attributes.sort(key=lambda k : (int(k['id'].name or 0), k['id'].size_id))
		print attributes
		for item in attributes:
			props = item['id'].properties
			if name == 'flash':
				props['pin_id'] = None
			child = node.addChild(name)
			child.setAttributes(self._getAttributeDictionaryFromId(props))
			child.setValue(item['value'])
	
	def addModuleAttributesToNode(self, node, peripheral, name, family=None):
		if family == None:
			family = 'stm32'
		attributes = self._getModuleAttributes()
		peripherals = []
		if isinstance(peripheral, list):
			peripherals.extend(peripheral)
		else:
			peripherals.append(peripheral)

		for item in attributes:
			instances = []
			attr = self._getAttributeDictionaryFromId(item['id'].properties)
			for p in peripherals:
				for module in [m for m in item['value'] if m.startswith(p)]:
					inst = module[len(p):]
					if inst != '':
						instances.append(inst)
			
			instances.sort(key=int)
			
			driver = node.addChild('driver')
			driver.setAttributes(attr)
			driver.setAttributes({'type': name, 'name': family})
			
			if len(instances) > 0:
				driver.setAttribute('instances', ",".join(instances))

	def addI2cToNode(self, node):
		self.addModuleAttributesToNode(node, 'I2C', 'i2c')

	def addSpiToNode(self, node):
		self.addModuleAttributesToNode(node, 'SPI', 'spi')

	def addAdcToNode(self, node):
		self.addModuleAttributesToNode(node, 'ADC', 'adc')

	def addDacToNode(self, node):
		self.addModuleAttributesToNode(node, 'DAC', 'dac')

	def addTimerToNode(self, node):
		self.addModuleAttributesToNode(node, 'TIM', 'timer')

	def addUartToNode(self, node):
		self.addModuleAttributesToNode(node, ['UART', 'USART'], 'uart')
		self.addModuleAttributesToNode(node, ['UART', 'USART'], 'spi', 'stm32_uart')
	
	def _getModuleAttributes(self):
		attributes = self.device.getAttributes('modules')
		newAttributes = attributes
		return newAttributes
	
	def addGpioToNode(self, node):
		attributes = self.device.getAttributes('gpios')
		driver = node.addChild('driver')
		driver.setAttributes({'type': 'gpio', 'name': 'stm32'})
		
		for item in attributes:
			gpios = item['value']
			gpios.sort(key=lambda k: k['port'])
			dict = self._getAttributeDictionaryFromId(item['id'].properties)
			for gpio in gpios:
				gpio_child = driver.addChild('gpio')
				gpio_child.setAttributes(dict)
				for name in ['port', 'id']:
					if name in gpio:
						gpio_child.setAttribute(name, gpio[name])
				for af in gpio['af']:
					af_child = gpio_child.addChild('af')
					af_child.setAttributes(af)
	
	def _getAttributeDictionaryFromId(self, props):
		target = props
		dict = {}
		for attr in target:
			if target[attr] != None and target[attr] != self.device.id.properties[attr]:
				if attr == 'size_id':
					dict['device-size-id'] = target[attr]
				if attr == 'name':
					dict['device-name'] = target[attr]
				if attr == 'pin_id':
					dict['device-pin-id'] = target[attr]
		return dict
	
	def write(self, folder):
		self.writeToFolder(folder, self.device.id.string + '.xml')

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "AVRDeviceWriter(\n" + self.toString() + ")"

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
		self.addDeviceAttributesToNode(self.root, 'linkerscript')
		self.addDeviceAttributesToNode(self.root, 'core')
		
		self.addDeviceAttributesToNode(self.root, 'pin-count')
		
		self.addDeviceAttributesToNode(self.root, 'header')
		self.addDeviceAttributesToNode(self.root, 'define')
		
		core_child = self.root.addChild('driver')
		core_child.setAttributes({'type': 'core', 'name': 'cortex'})
		param_name_value = {
#							'enable_hardfault_handler': 'false',
#							'vector_table_in_ram': 'false',
							'allocator': 'newlib'}
		for param_name in param_name_value:
			param_core_child = core_child.addChild('parameter')
			param_core_child.setAttributes({'name': param_name})
			param_core_child.setValue(param_name_value[param_name])

		# ADC
		self.addModuleAttributesToNode(self.root, 'ADC', 'adc')
		# CAN
		self.addModuleAttributesToNode(self.root, 'CAN', 'can')
		# Clock
		clock_child = self.root.addChild('driver')
		clock_child.setAttributes({'type': 'clock', 'name': 'stm32'})
		# DAC
		self.addModuleAttributesToNode(self.root, 'DAC', 'dac')
		# I2C
		self.addModuleAttributesToNode(self.root, 'I2C', 'i2c')
		# SPI
		self.addModuleAttributesToNode(self.root, 'SPI', 'spi')
		self.addModuleAttributesToNode(self.root, ['UART', 'USART'], 'spi', 'stm32_uart')
		# Timer
		self.addModuleAttributesToNode(self.root, 'TIM', 'timer')
		# UART
		self.addModuleAttributesToNode(self.root, ['UART', 'USART'], 'uart')
		# GPIO
		self.addGpioToNode(self.root)
		

	def addDeviceAttributesToNode(self, node, name):
		attributes = self.device.getAttributes(name)
		if name in ['flash', 'ram', 'pin-count', 'linkerscript']:
			seen = set()
			attributes = [a for a in attributes if a['value'] not in seen and not seen.add(a['value'])]
		attributes.sort(key=lambda k : (int(k['id'].name or 0), k['id'].size_id))
		for item in attributes:
			props = item['id'].properties
			if name  in ['flash', 'ram', 'linkerscript']:
				props['pin_id'] = None
			if name  in ['pin-count']:
				props['size_id'] = None
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
					for id in ['id', 'peripheral', 'name', 'type']:
						if id in af:
							af_child.setAttribute(id, af[id])
	
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
		file_name = 'stm32f' + self.device.id.name.replace('|', '_')
		file_name += '-' + self.device.id.pin_id.replace('|', '_')
		file_name += '-' + self.device.id.size_id.replace('|', '_')
		self.writeToFolder(folder, file_name + '.xml')

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "STMDeviceWriter(\n" + self.toString() + ")"

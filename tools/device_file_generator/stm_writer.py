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
		# USB
		self.addModuleAttributesToNode(self.root, 'USB', 'usb', 'stm32_fs')
		# GPIO
		self.addGpioToNode(self.root)
		

	def addDeviceAttributesToNode(self, node, name):
		properties = self.device.getProperty(name)
		
		if properties == None:
			return
		
		for prop in properties.values:
			for id in prop.ids.differenceFromIds(self.device.ids):
				attr = self._getAttributeDictionaryFromId(id)
				child = node.addChild(name)
				child.setAttributes(attr)
				child.setValue(prop.value)
	
	def addModuleAttributesToNode(self, node, peripheral, name, family=None):
		if family == None:
			family = 'stm32'
		modules = self.device.getProperty('modules')
		
		peripherals = []
		if isinstance(peripheral, list):
			peripherals.extend(peripheral)
		else:
			peripherals.append(peripheral)

		for prop in modules.values:
			instances = []
			for p in peripherals:
				for module in [m for m in prop.value if m.startswith(p)]:
					inst = module[len(p):]
					if inst != '' and inst.isdigit():
						instances.append(inst)
			
			if len(instances) == 0:
				continue
			instances.sort(key=int)
			
			for id in prop.ids.differenceFromIds(self.device.ids):
				attr = self._getAttributeDictionaryFromId(id)
			
				driver = node.addChild('driver')
				driver.setAttributes(attr)
				driver.setAttributes({'type': name, 'name': family})
				
				if len(instances) > 0:
					driver.setAttribute('instances', ",".join(instances))
					
	
	def addGpioToNode(self, node):
		props = self.device.getProperty('gpios')
		
		driver = node.addChild('driver')
		driver.setAttributes({'type': 'gpio', 'name': 'stm32'})
		
		for prop in props.values:
			gpios = prop.value
			gpios.sort(key=lambda k: (k['port'],int(k['id'])))
			
			for id in prop.ids.differenceFromIds(self.device.ids):
				dict = self._getAttributeDictionaryFromId(id)
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
	
	def _getAttributeDictionaryFromId(self, id):
		target = id.properties
		dict = {}
		for attr in target:
			if target[attr] != None:
				if attr == 'size_id':
					dict['device-size-id'] = target[attr]
				if attr == 'name':
					dict['device-name'] = target[attr]
				if attr == 'pin_id':
					dict['device-pin-id'] = target[attr]
		return dict
	
	def write(self, folder):
		file_name = 'stm32f' + '_'.join(self.device.ids.getAttribute('name'))
		file_name += '-' + '_'.join(self.device.ids.getAttribute('pin_id'))
		file_name += '-' + '_'.join(self.device.ids.getAttribute('size_id'))
		self.writeToFolder(folder, file_name + '.xml')

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "STMDeviceWriter(\n" + self.toString() + ")"

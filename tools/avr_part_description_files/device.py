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
from device_element import DeviceElementBase, DeviceString

# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

class PartDescriptionFile:
	""" PartDescriptionFile
	Represents a device in xml format.
	"""

	def __init__(self, xml_file, logger=None):
		node = self._openDeviceXML(xml_file)
		
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		self.device = node.findall('devices')[0][0]
		self.name = self.device.get('name')
		self.architecture = self.device.get('architecture')
		self.family = self.device.get('family')
		self.log.info("Parsing AVR PDF: " + self.name)
		
		self.properties = {}
		self.properties['interrupts'] = []
		self.properties['modules'] = []
		self.modules = []
		self.gpios = []
		
		if (self.architecture != 'AVR8' and self.architecture != 'AVR8L'):
			self.log.error("Only ATtiny, ATmega and AT90 targets can correctly be parsed...")
			return None
		
		# find the values for flash, ram and (optional) eeprom
		for memory_segment in self.device.iter('memory-segment'):
			name = memory_segment.get('name')
			size = int(memory_segment.get('size'), 16)
			if name == 'FLASH' or name == 'APP_SECTION':
				self.properties['flash'] = size
				self.log.debug("FLASH: " + str(size))
			elif name == 'IRAM' or name == 'SRAM' or name == 'INTERNAL_SRAM':
				self.properties['ram'] = size
				self.log.debug("RAM: " + str(size))
			elif name == 'EEPROM':
				self.properties['eeprom'] = size
				self.log.debug("EEPROM: " + str(size))
		
		# if flash or ram is missing, it is a bad thing and unsupported
		if 'flash' not in self.properties:
			self.log.error("No FLASH found")
			return None
		if 'ram' not in self.properties:
			self.log.error("No RAM found")
			self.log.error("XPCC does not support Assembler-only programming")
			return None
		# eeprom is optional on AVR and not available on ARM devices
		if 'eeprom' not in self.properties and 'AVR' in self.architecture:
			self.log.warn("No EEPROM found")
			
		
		self.modules = node.findall('modules')[0]
		# these modules are either too complicated or too special to bother with
		ignore_modules = ['LOCKBIT', 'FUSE', 'EEPROM', 'CPU', 'WATCHDOG', 'BOOT_LOAD', 'PLL', 'USB_DEVICE', 'PS2']
		
		for module in self.modules.iter('module'):
			name = module.get('name')
			if name not in ignore_modules:
				self.properties['modules'].append(name)
			# parse the GPIOs
			if "PORT" in name:
				gpio = self._gpioFromModuleNode(module)
				if gpio == None:
					self.log.warn("No GPIO found for " + name)
				else:
					self.gpios.append(gpio);
					self.log.debug("GPIOs: " + str(gpio))

	def _openDeviceXML(self, filename):
		try:
			# parse the xml-file
			xmltree = et.parse(filename).getroot()
		except OSError as e:
			raise ParserException(e)
		except (xml.parsers.expat.ExpatError, xml.etree.ElementTree.ParseError) as e:
			raise ParserException("while parsing xml-file '%s': %s" % (filename, e))
		return xmltree

	def _gpioFromModuleNode(self, node):
		name = node.get('name')
		port = name[4:5]
		for c in node.iter('register'):
			if name == c.get('name'): 
				mask = self._maskFromRegisterNode(c)
				return {'port': port, 'mask': mask}
		return None
	
	def _maskFromRegisterNode(self, node):
		mask = node.get('mask')
		# some registers have bitfields instead of masks
		# Go home Atmel, you're drunk.
		if mask == None:
			mask = 0
			for c in node.iter('bitfield'):
				field = c.get('mask')
				if field != None:
					field = int(field, 16)
					mask |= field
		else:
			mask = int(mask, 16)
		return mask

	def __str__(self):
		s  = "Architecture: " + self.architecture + "\n"
		s += "Family: " + self.family + "\n"
		s += "Name: " + self.name
		return s

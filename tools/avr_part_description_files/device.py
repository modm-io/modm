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
		
		self.properties = {}
		for memory_segment in self.device.iter('memory-segment'):
			name = memory_segment.get('name')
			size = int(memory_segment.get('size'), 16)
			if name == 'FLASH':
				self.properties['flash'] = size
			elif name == 'IRAM' or name == 'SRAM':
				self.properties['ram'] = size
			elif name == 'EEPROM':
				self.properties['eeprom'] = size
			
		self.properties['interrupts'] = []
		for module in self.device.iter('interrupt'):
			self.properties['interrupts'].append(module.attrib)
			
		self.modules = node.findall('modules')[0]
		self.properties['modules'] = []
		self.gpios = []
		ignore_modules = ['LOCKBIT', 'FUSE', 'EEPROM', 'CPU', 'WATCHDOG', 'BOOT_LOAD', 'PLL', 'USB_DEVICE', 'PS2']
		
		for module in self.modules.iter('module'):
			name = module.get('name')
			if name not in ignore_modules:
				self.properties['modules'].append(name)
			if "PORT" in name:
				self.gpios.append(self._gpiosFromModuleNode(module));

	def _openDeviceXML(self, filename):
		try:
			# parse the xml-file
			xmltree = et.parse(filename).getroot()
		except OSError as e:
			raise ParserException(e)
		except (xml.parsers.expat.ExpatError, xml.etree.ElementTree.ParseError) as e:
			raise ParserException("while parsing xml-file '%s': %s" % (filename, e))
		return xmltree

	def _gpiosFromModuleNode(self, node):
		ports = []
		for c in node.iter('register'):
			if "PORT" in c.get('name'): 
				name = c.get('name')[-1:]
				mask = self._maskFromRegisterNode(c)
				for i in range(8):
					if mask & 0x01:
						ports.append( {'port': name, 'id': i} )
					mask >>= 1
		return ports
	
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

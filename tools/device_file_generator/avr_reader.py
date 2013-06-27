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
from peripheral import Peripheral
from register import Register
import avr_io

import os, sys, math
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger
# add python module device files to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))
from device_identifier import DeviceIdentifier

class AVRDeviceReader(XMLDeviceReader):
	""" AVRDeviceReader
	This AVR specific part description file reader knows the structure and
	translates the data into a platform independent format.
	"""

	def __init__(self, file, logger=None):
		XMLDeviceReader.__init__(self, file, logger)

		device = self.query("//device")[0]
		self.name = device.get('name')
		architecture = device.get('architecture')
		family = device.get('family')
		
		dev = DeviceIdentifier(self.name.lower())
		self.properties['id'] = dev
		self.properties['mmcu'] = dev.family + dev.name
		if dev.type != None:
			self.properties['mmcu'] += dev.type
		else:
			dev.type = "none"
		self.properties['core'] = architecture.lower()

		self.log.info("Parsing AVR PDF: " + architecture + " " + self.name)

		if (architecture != 'AVR8' and architecture != 'AVR8L'):
			self.log.error("Only ATtiny, ATmega and AT90 targets can correctly be parsed...")
			return None

		self.properties['define'] = "__AVR_" + self.name + "__"

		# find the values for flash, ram and (optional) eeprom
		for memory_segment in self.query("//memory-segment"):
			name = memory_segment.get('name')
			size = int(memory_segment.get('size'), 16)
			if name == 'FLASH' or name == 'APP_SECTION':
				self.properties['flash'] = size
			elif name == 'IRAM' or name == 'SRAM' or name == 'INTERNAL_SRAM':
				self.properties['ram'] = size
			elif name == 'EEPROM':
				self.properties['eeprom'] = size

		self.properties['gpios'] = gpios = []
		self.properties['peripherals'] = peripherals = []
		self.properties['modules'] = modules = []
		
		self.modules = self.compactQuery("//module/@name")
		self.log.debug("Available Modules are:\n" + self._modulesToString())

		for mod in self.query("//modules/module"):
			name = mod.get('name')
			
			if "PORT" in name:
				module = self.createModule(name)
				gpios.extend(self._gpioFromModule(module))
				continue
			
			if any(name.startswith(per) for per in ["EXTERNAL_INT", "TWI", "USART", "SPI", "AD_CON", "USB", "CAN", "DA_CON", "USI", "TIMER"]):
				modules.append(name)
				module = self.createModule(name)
				peripherals.append(module)
				continue
		
		for pin_array in [a for a in avr_io.pins if self.properties['mmcu'] in a['devices']]:
			for pcint in pin_array['pcint']:
				for gpio in gpios:
					if gpio['port'] == pcint['port'] and gpio['id'] == pcint['id']:
						gpio['pcint'] = pcint['int']
			for exti in pin_array['exti']:
				for gpio in gpios:
					if gpio['port'] == exti['port'] and gpio['id'] == exti['id']:
						gpio['extint'] = exti['int']
	
	def createModule(self, name):
		if name in self.modules:
			return Peripheral(name, self._registersOfModule(name), self.log)
		else:
			self.log.error("'" + name + "' not a module!")
			self.log.error("Available modules are:\n" + self._modulesToString())
			return None

	def _registersOfModule(self, module):
		results = self.query("//register-group[@name='" + module + "']/register")
		registers = []
		for res in results:
			registers.append(self._translateRegister(res))
		return registers
	
	def _translateRegister(self, queryResult):
		mask = queryResult.get('mask')
		size = int(queryResult.get('size'))
		name = queryResult.get('name')
		register = Register(name)
		register.size = size
		
		if mask == None:
			fields = queryResult.findall('bitfield')
			for field in fields:
				fname = field.get('name')
				if 'Res' in fname:
					continue
				
				fmask = int(field.get('mask'), 16)
				flsb = field.get('lsb')
				if flsb == None:
					flsb = 0
				else:
					flsb = int(flsb)
					
				if bin(fmask).count("1") > 1:
					start = flsb
					for iii in range(8*size):
						if fmask & 2**(iii):
							name = fname + str(start)
							start += 1
							register.addField(name, iii)
				else:
					register.addField(fname, int(math.log(fmask, 2)))
		else:
			fmask = int(mask, 16)
			flsb = queryResult.get('lsb')
			if flsb == None:
				flsb = 0
			else:
				flsb = int(flsb)
				
			if bin(fmask).count("1") > 1:
				start = flsb
				for iii in range(8*size):
					if fmask & 2**(iii):
						register.addField('data'+str(start), iii)
						start += 1
		
		return register

	def _modulesToString(self):
		string = ""
		char = self.modules[0][0:1]
		for module in self.modules:
			if not module.startswith(char):
				string += "\n"
			string += module + " \t"
			char = module[0][0:1]
		return string

	def _gpioFromModule(self, module):
		"""
		This tries to get information about available pins in a port and
		returns a dictionary containing the port name and available pins
		as a bit mask.
		"""
		port = module.name[4:5]
		for reg in module.registers:
			if module.name == reg.name: 
				mask = reg.maskFromRegister()
				return self._getAttributedPortDictionary({'port': port, 'mask': mask})
		return None
	
	def _getAttributedPortDictionary(self, port):
		ports = []
		mask = port['mask']
		id = 0
		
		while id < 16:
			if mask & 0x01:
				ports.append({'port': port['port'], 'id': str(id)})
			mask >>= 1
			id += 1
		
		return ports

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "AVRDeviceReader(" + self.name + ")"

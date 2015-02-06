# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

from reader import XMLDeviceReader
from peripheral import Peripheral
from register import Register
import avr_io
import avr_mcu

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
		self.mcu_devices = [{"device": DeviceIdentifier(d['device']), "mcu": d['mcu']} for d in avr_mcu.mcu]

		self.log.info("Parsing AVR PDF: %s %s" % (architecture, self.name))

		dev = DeviceIdentifier(self.name.lower())
		self.id = dev

		mcu = self._findMcuFromDevice(self.id)
		if mcu == None:
			mcu = 'unsupported'
		else:
			mcu = mcu['mcu']

		dev_name = dev.family
		if dev.family == 'at90':
			dev_name += dev.type + dev.name
		elif dev.family == 'xmega':
			dev_name = 'atxmega' + dev.name
			dev_name += dev.type
			if dev.pin_id:
				dev_name += dev.pin_id
			else:
				dev.pin_id = 'none'
		else: # atmega
			dev_name += dev.name
			if dev.type:
				dev_name += dev.type
			else:
				dev.type = 'none'

		self.addProperty('mcu', mcu)
		self.addProperty('pin-name', dev_name)


		self.addProperty('core', architecture.lower())
		if (architecture not in ['AVR8', 'AVR8L', 'AVR8_XMEGA']):
			self.log.error("Sorry, only ATtiny, ATmega, ATxmega and AT90 targets can be parsed corretly.")
			exit()

		self.addProperty('define', '__AVR_' + self.name + '__')

		# find the values for flash, ram and (optional) eeprom
		for memory_segment in self.query('//memory-segment'):
			name = memory_segment.get('name')
			size = int(memory_segment.get('size'), 16)
			if name in ['FLASH', 'APP_SECTION']:
				self.addProperty('flash', size)
			elif name in ['IRAM', 'SRAM', 'INTERNAL_SRAM']:
				self.addProperty('ram', size)
			elif name == 'EEPROM':
				self.addProperty('eeprom', size)

		gpios = []
		self.addProperty('gpios', gpios)
		peripherals = []
		self.addProperty('peripherals', peripherals)
		modules = []
		self.addProperty('modules', modules)

		self.modules = self.compactQuery("//peripherals/module/instance/@name")
		self.log.debug("Available Modules are:\n" + self._modulesToString())

		if dev.family == 'xmega':
			for dev in [d for d in avr_io.xmega_pins if d['type'] == dev.type]:
				for port in dev['gpio']:
					port_dict = self._getAttributedPortDictionary(port)
					gpios.extend(port_dict)

			for mod in self.query("//peripherals/module/instance"):
				name = mod.get('name')

				if any(name.startswith(per) for per in ["TWI", "USART", "SPI", "ADC", "USB", "DAC", "TC"]):
					if not name.endswith("SPI"):
						modules.append(name)
						continue

			for peripheral in modules:
				base = None
				for per in ["TWI", "USART", "SPI", "ADC", "USB", "DAC", "TC"]:
					if peripheral.startswith(per):
						base = per
						break
				if base:
					port = peripheral.replace(base, "")
					base = base.lower()
					instance = None
					if len(port) == 2:
						instance = port[1:]
						port = port[:1]
					if base in avr_io.xmega_peripheral_pins:
						module = list(avr_io.xmega_peripheral_pins[base])
						for pin in [p for p in module if instance == None or instance == p["instance"]]:
							for gpio in [g for g in gpios if g['port'] == port and g['id'] == pin['id']]:
								if base == 'twi':
									af = {'peripheral' : 'I2cMaster' + port,
										  'name': pin['name'].capitalize(),
										  'type': pin['dir']}
									gpio['af'].append(af)
								elif base == 'spi':
									af = {'peripheral' : 'SpiMaster' + port,
										  'name': pin['name'].capitalize(),
										  'type': pin['dir']}
									if 'remap' in pin:
										af.update({'remap': pin['remap']})
									gpio['af'].append(af)
									negate = {'in': 'out', 'out': 'in', 'io': 'io'}
									repl = {'mosi': 'simo', 'miso': 'somi', 'sck': 'sck', 'ss': 'ss'}
									af2 = {'peripheral' : 'SpiSlave' + port,
										  'name': repl[pin['name']].capitalize(),
										  'type': negate[pin['dir']]}
									if 'remap' in pin:
										af2.update({'remap': pin['remap']})
									gpio['af'].append(af2)
								elif base == 'usart':
									af = {'peripheral' : 'Uart' + port + instance,
										  'name': pin['name'].capitalize(),
										  'type': pin['dir']}
									if 'remap' in pin:
										af.update({'remap': pin['remap']})
									gpio['af'].append(af)
									repl = {'txd': 'mosi', 'rxd': 'miso', 'xck': 'sck'}
									af = {'peripheral' : 'UartSpiMaster' + port + instance,
										  'name': repl[pin['name']].capitalize(),
										  'type': pin['dir']}
									if 'remap' in pin:
										af.update({'remap': pin['remap']})
									gpio['af'].append(af)
								elif base == 'tc':
									af = {'peripheral' : 'TimerCounter' + port + instance,
										  'name': pin['name'].capitalize(),
										  'type': pin['dir']}
									if 'remap' in pin:
										af.update({'remap': pin['remap']})
									gpio['af'].append(af)

		else:
			for mod in self.query("//modules/module"):
				name = mod.get('name')

				if "PORT" in name:
					module = self.createModule(name)
					gpios.extend(self._gpioFromModule(module))

				if any(name.startswith(per) for per in ["EXTERNAL_INT", "TWI", "USART", "SPI", "AD_CON", "USB", "CAN", "DA_CON", "TIMER", "PORT"]):
					modules.append(name)
					module = self.createModule(name)
					peripherals.append(module)
					continue

			for pin_array in [a for a in avr_io.pins if dev_name in a['devices']]:
				for name in ['pcint', 'extint']:
					if name in pin_array:
						for module in pin_array[name]:
							for gpio in [g for g in gpios if g['port'] == module['port'] and g['id'] == module['id']]:
								gpio[name] = module['int']

				for name in ['spi', 'i2c']:
					if name in pin_array:
						for module in pin_array[name]:
							for gpio in [g for g in gpios if g['port'] == module['port'] and g['id'] == module['id']]:
								if name == 'i2c':
									af = {'peripheral' : 'I2cMaster',
										  'name': module['name'].capitalize(),
										  'type': module['dir']}
									gpio['af'].append(af)
								elif name == 'spi':
									af = {'peripheral' : 'SpiMaster',
										  'name': module['name'].capitalize(),
										  'type': module['dir']}
									gpio['af'].append(af)
									negate = {'in': 'out', 'out': 'in', 'io': 'io'}
									repl = {'mosi': 'simo', 'miso': 'somi', 'sck': 'sck', 'ss': 'ss'}
									af2 = {'peripheral' : 'SpiSlave',
										  'name': repl[module['name']].capitalize(),
										  'type': negate[module['dir']]}
									gpio['af'].append(af2)
								# elif name == 'usi':
								# 	af = {'peripheral' : 'Usi',
								# 		  'name': module['name'].capitalize(),
								# 		  'type': module['dir']}
								# 	gpio['af'].append(af)

				for name in ['uart0', 'uart1', 'uart2', 'uart3']:
					if name in pin_array:
						for module in pin_array[name]:
							for gpio in [g for g in gpios if g['port'] == module['port'] and g['id'] == module['id']]:
								af = {'peripheral' : name.capitalize(),
									  'name': module['name'].capitalize(),
									  'type': module['dir']}
								gpio['af'].append(af)
								if 'uartspi' in pin_array:
									repl = {'txd': 'mosi', 'rxd': 'miso', 'xck': 'sck'}
									af = {'peripheral' : 'UartSpiMaster'+name[4:],
										  'name': repl[module['name']].capitalize(),
										  'type': module['dir']}
									gpio['af'].append(af)

	def _findMcuFromDevice(self, device):
		d = DeviceIdentifier(device)
		sub_match = []
		for dev in self.mcu_devices:
			id = dev['device']
			if id in d:
				return dev

			if id.family == d.family and id.name == d.name:
				sub_match.append(dev)

		if len(sub_match) == 1:
			return sub_match[0]
		if d.type == 'a':
			d.type = None
		elif d.type == 'pa':
			d.type = 'p'

		for dev in sub_match:
			if dev["device"] in d:
				return dev

		return None

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
				ports.append({'port': port['port'], 'id': str(id), 'af': []})
			mask >>= 1
			id += 1

		return ports

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "AVRDeviceReader(" + self.name + ")"

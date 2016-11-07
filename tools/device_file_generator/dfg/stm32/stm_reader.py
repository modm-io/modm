#!/usr/bin/env pythonNone
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, Niklas Hauser
# Copyright (c) 2016, Fabian Greif
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import re

from lxml import etree

from logger import Logger
from device_identifier import DeviceIdentifier

from ..reader import XMLDeviceReader

from .stm import stm32_defines
from .stm import stm32f1_remaps
from .stm import stm32_memory


class STMDeviceReader(XMLDeviceReader):
	""" STMDeviceReader
	This STM specific part description file reader knows the structure and
	translates the data into a platform independent format.
	"""
	familyFile = None
	rootpath = None

	@staticmethod
	def getDevicesFromFamily(family, logger=None, rootpath=None):
		if rootpath is None:
			rootpath = os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', '..', 'STM_devices', 'mcu')
		STMDeviceReader.rootpath = rootpath

		STMDeviceReader.familyFile = XMLDeviceReader(os.path.join(rootpath, 'families.xml'), logger)
		rawDevices = STMDeviceReader.familyFile.query("//Family[@Name='{}']/SubFamily/Mcu/@RefName".format(family))

		# devices can contain duplicates due to Hx, Tx, Yx, Ix suffix!
		# we treat them as single devices, since we don't care about the MCUs package
		devices = []
		for dev in rawDevices:
			shortDev = dev[:-2] if dev.endswith('x') else dev
			if all(not d.startswith(shortDev) for d in devices):
				devices.append(dev)

		logger.debug("STMDeviceReader: Found devices of family '{}': {}".format(family, ", ".join(devices)))
		return devices


	def __init__(self, deviceName, logger=None):
		deviceNames = self.familyFile.query("//Family/SubFamily/Mcu[@RefName='{}']".format(deviceName))[0]
		comboDeviceName = deviceNames.get('Name')
		deviceFile = os.path.join(self.rootpath, comboDeviceName + '.xml')

		XMLDeviceReader.__init__(self, deviceFile, logger)
		self.name = deviceName
		self.id = DeviceIdentifier(self.name.lower())

		if logger:
			logger.info("STMDeviceReader: Parsing '{}'".format(self.id.string))

		# information about the core and architecture
		coreLut = {'m0': 'v6m', 'm3': 'v7m', 'm4': 'v7em', 'm7': 'v7em'}
		core = self.query('//Core')[0].text.replace('ARM ', '').lower()
		self.addProperty('architecture', coreLut[core.replace('cortex-', '')])
		if core.endswith('m4') or core.endswith('m7'):
			core += 'f'
		if self.id.family in ['f7'] and self.id.name not in ['745', '746', '756']:
			core += 'd'
		self.addProperty('core', core)

		# flash and ram sizes
		# The <ram> and <flash> can occur multiple times.
		# they are "ordered" in the same way as the `(S-I-Z-E)` ids in the device combo name
		# we must first find out which index the current self.id.size_id has inside `(S-I-Z-E)`
		sizeIndexFlash = 0
		sizeIndexRam = 0

		matchString = "\(.(-.)*\)"
		match = re.search(matchString, comboDeviceName)
		if match:
			sizeArray = match.group(0)[1:-1].lower().split("-")
			sizeIndexFlash = sizeArray.index(self.id.size_id)
			sizeIndexRam = sizeIndexFlash

		rams = self.query("//Ram")
		if len(rams) <= sizeIndexRam:
			sizeIndexRam = 0

		flashs = self.query("//Flash")
		if len(flashs) <= sizeIndexFlash:
			sizeIndexFlash = 0

		mem_fam = stm32_memory[self.id.family]
		mem_model = None
		for model in mem_fam['model']:
			if any(name.startswith(self.id.name) for name in model['names']):
				if self.id.name in model['names']:
					mem_model = model
					break
				elif "{}x{}".format(self.id.name, self.id.size_id) in model['names']:
					mem_model = model
					break
		if mem_model == None:
			self.log.error("STMDeviceReader: Memory model not found for device '{}'".format(self.id.string))

		total_ram = ram = int(rams[sizeIndexRam].text) + mem_model['memories']['sram1']
		flash = int(flashs[sizeIndexFlash].text) + mem_model['memories']['flash']
		if 'ccm' in mem_model['memories']:
			total_ram += mem_model['memories']['ccm']
		if 'backup' in mem_model['memories']:
			total_ram += mem_model['memories']['backup']
		if 'itcm' in mem_model['memories']:
			total_ram += mem_model['memories']['itcm']
			total_ram += mem_model['memories']['dtcm']
		self.addProperty('ram', total_ram * 1024)
		self.addProperty('flash', flash * 1024)

		memories = []
		# first get the real SRAM1 size
		for mem, val in mem_model['memories'].items():
			if any(s in mem for s in ['2', '3', 'dtcm']):
				ram -= val

		# add all memories
		for mem, val in mem_model['memories'].items():
			if '1' in mem:
				memories.append({'name': 'sram1',
								 'access' : 'rwx',
								 'start': "0x{:02X}".format(mem_fam['start']['sram']),
								 'size': str(ram)})
			elif '2' in mem:
				memories.append({'name': 'sram2',
								 'access' : 'rwx',
								 'start': "0x{:02X}".format(mem_fam['start']['sram'] + ram * 1024),
								 'size': str(val)})
			elif '3' in mem:
				memories.append({'name': 'sram3',
								 'access': 'rwx',
								 'start': "0x{:02X}".format(mem_fam['start']['sram'] + ram * 1024 + mem_model['memories']['sram2'] * 1024),
								 'size': str(val)})
			elif 'flash' in mem:
				memories.append({'name': 'flash',
								 'access': 'rx',
								 'start': "0x{:02X}".format(mem_fam['start']['flash']),
								 'size': str(flash)})
			else:
				memories.append({'name': mem,
				                 'access': 'rw' if self.id.family == 'f4' and mem == 'ccm' else 'rwx',
								 'start': "0x{:02X}".format(mem_fam['start'][mem]),
								 'size': str(val)})

		self.addProperty('memories', memories)

		# packaging
		package = self.query("//@Package")[0]
		self.addProperty('pin-count', re.findall('[0-9]+', package)[0])
		self.addProperty('package', re.findall('[A-Za-z\.]+', package)[0])

		# device header
		self.addProperty('header', 'stm32' + self.id.family + 'xx.h')

		# device defines
		defines = []
		if self.id.family in ['f4']:
			# required for our FreeRTOS
			defines.append('STM32F4XX')

		dev_def = self._getDeviceDefine()
		if dev_def is None:
			logger.error("STMDeviceReader: Define not found for device '{}'".format(self.id.string))
		else:
			defines.append(dev_def)

		self.addProperty('define', defines)


		gpios = []
		self.addProperty('gpios', gpios)
		gpio_afs = []
		self.addProperty('gpio_afs', gpio_afs)
		peripherals = []
		self.addProperty('peripherals', peripherals)
		modules = []
		self.addProperty('modules', modules)

		self.modules = self.query("//IP/@InstanceName")
		self.modules = sorted(list(set(self.modules)))
		self.log.debug("STMDeviceReader: Available Modules are:\n" + self._modulesToString())

		# add entire interrupt vectore table here.
		# I have not found a way to extract the correct vector _position_ from the ST device files
		# so we have to swallow our pride and just parse the header file
		# ext/cmsis/stm32/Device/ST/STM32F4xx/Include/
		headerFilePath = os.path.join('..', '..', 'ext', 'st', 'stm32{}xx'.format(self.id.family), 'Include', '{}.h'.format(dev_def.lower()))
		headerFile = open(headerFilePath, 'r').read()
		match = re.search("typedef enum.*?/\*\*.*?/\*\*.*?\*/(?P<table>.*?)} IRQn_Type;", headerFile, re.DOTALL)
		if not match:
			logger.error("STMDeviceReader: Interrupt vector table not found for device '{}'".format(self.id.string))
			exit(1)

		# print dev_def.lower(), match.group('table')

		ivectors = []
		for line in match.group('table').split('\n')[1:-1]:
			if '=' not in line:  # avoid multiline comment
				continue

			name, pos = line.split('/*!<')[0].split('=')
			pos = int(pos.strip(' ,'))
			name = name.strip()[:-5]
			if self.id.family in ['f3'] and pos == 42 and name == 'USBWakeUp':
				continue
			ivectors.append({'position': pos, 'name': name})

		self.log.debug("STMDeviceReader: Found interrupt vectors:\n" + "\n".join(["{}: {}".format(v['position'], v['name']) for v in ivectors]))
		self.addProperty('interrupts', ivectors)

		for m in self.modules:
			if any(m.startswith(per) for per in ['TIM', 'UART', 'USART', 'ADC', 'CAN', 'SPI', 'I2C', 'FSMC', 'FMC', 'RNG', 'RCC', 'USB']):
				modules.append(m)

		if 'CAN' in modules:
			modules.append('CAN1')

		if self.id.family in ['f2', 'f3', 'f4', 'f7']:
			modules.append('ID')

		self.dmaFile = None
		if 'DMA' in self.modules:
			# lets load additional information about the DMA
			dma_file = self.query("//IP[@Name='DMA']")[0].get('Version')
			dma_file = os.path.join(self.rootpath, 'IP', 'DMA-' + dma_file + '_Modes.xml')
			self.dmaFile = XMLDeviceReader(dma_file, logger)
			dmas = [d.get('Name') for d in self.dmaFile.query("//IP/ModeLogicOperator/Mode[starts-with(@Name,'DMA')]")]
			modules.extend(dmas)

		invertMode = {'out': 'in', 'in': 'out', 'io': 'io'}
		nameToMode = {'rx': 'in', 'tx': 'out', 'cts': 'in', 'rts': 'out', 'ck': 'out',  # Uart
					 'miso': 'in', 'mosi': 'out', 'nss': 'io', 'sck': 'out',  # Spi
					 'scl': 'out', 'sda': 'io'}  # I2c

		# lets load additional information about the GPIO IP
		ip_file = self.query("//IP[@Name='GPIO']")[0].get('Version')
		ip_file = os.path.join(self.rootpath, 'IP', 'GPIO-' + ip_file + '_Modes.xml')
		self.gpioFile = XMLDeviceReader(ip_file, logger)

		pins = self.query("//Pin[@Type='I/O'][starts-with(@Name,'P')]")
		pins = sorted(pins, key=lambda p: [p.get('Name')[1:2], int(p.get('Name')[:4].split('-')[0].split('/')[0][2:])])

		for pin in pins:
			name = pin.get('Name')
			# F1 does not have pin 'alternate functions' only pin 'remaps' which switch groups of pins
			if self.id.family == 'f1':
				pinSignals = self.gpioFile.compactQuery("//GPIO_Pin[@Name='{}']/PinSignal/RemapBlock/..".format(name))
				rawAltFunctions = {a.get('Name'): a[0].get('Name')[-1:] for a in pinSignals}
				altFunctions = {}
				for alt_name in rawAltFunctions:
					key = alt_name.split('_')[0].lower()
					if key not in stm32f1_remaps:
						key += alt_name.split('_')[1].lower()
					if key in stm32f1_remaps:
						mask = stm32f1_remaps[key]['mask']
						pos = stm32f1_remaps[key]['position']
						value = stm32f1_remaps[key]['mapping'][int(rawAltFunctions[alt_name])]
						altFunctions[alt_name] = '{},{},{}'.format(pos, mask, value)
				# Add the rest of the pins
				allSignals = self.compactQuery("//Pin[@Name='{}']/Signal".format(name))
				for sig in allSignals:
					if not any(sig.get('Name') in name.get('Name') for name in pinSignals):
						pinSignals.append(sig)

			else:  # F0, F3, F4 and F7
				pinSignals = self.gpioFile.compactQuery("//GPIO_Pin[@Name='%s']/PinSignal/SpecificParameter[@Name='GPIO_AF']/.." % name)
				altFunctions = { a.get('Name') : a[0][0].text.replace('GPIO_AF', '')[:2].replace('_', '') for a in pinSignals }

				# the analog channels are only available in the Mcu file, not the GPIO file
				analogSignals = self.compactQuery("//Pin[@Name='{}']/Signal[starts-with(@Name,'ADC')]".format(name))
				pinSignals.extend(analogSignals)

			name = name[:4].split('-')[0].split('/')[0].strip()

			gpio = {'port': name[1:2], 'id': name[2:]}
			gpios.append(gpio)

			afs = []

			for signal in [s.get('Name') for s in pinSignals]:
				raw_names = signal.split('_')
				if len(raw_names) < 2:
					continue

				if not any(m.startswith(raw_names[0]) for m in modules):
					continue

				instance = raw_names[0][-1]
				if not instance.isdigit():
					instance = ""

				name = raw_names[1].lower()
				mode = None
				if name in nameToMode and nameToMode[name] != 'io':
					mode = nameToMode[name]
				af_id = None
				if signal in altFunctions:
					af_id = altFunctions[signal]

				if signal.startswith('USART') or signal.startswith('UART'):
					af = {'peripheral' : 'Uart' + instance,
						  'name': name.capitalize()}
					if mode:
						af.update({'type': mode})
					if af_id:
						af.update({'id': af_id})
					afs.append(af)

					mapName = {'rx': 'miso', 'tx': 'mosi', 'ck': 'sck'}
					if signal.startswith('USART') and name in mapName:
						af = {'peripheral' : 'UartSpiMaster' + instance,
							  'name': mapName[name].capitalize()}
						if mode:
							af.update({'type': mode})
						if af_id:
							af.update({'id': af_id})
						afs.append(af)

				elif signal.startswith('SPI'):
					af = {'peripheral' : 'SpiMaster' + instance,
						  'name': name.capitalize()}
					if mode:
						af.update({'type': mode})
					if af_id:
						af.update({'id': af_id})
					afs.append(dict(af))
					# invertName = {'miso': 'somi', 'mosi': 'simo', 'nss': 'nss', 'sck': 'sck'}
					# af.update({	'peripheral' : 'SpiSlave' + instance,
					# 			'name': invertName[name].capitalize()})
					# if mode:
					# 	af.update({'type': invertMode[nameToMode[name]]})
					# afs.append(af)

				if signal.startswith('CAN'):
					if instance == '':
						instance = '1'
					af = {'peripheral' : 'Can' + instance,
						  'name': name.capitalize()}
					if mode:
						af.update({'type': mode})
					if af_id:
						af.update({'id': af_id})
					afs.append(af)

				if signal.startswith('RCC'):
					if 'MCO' in signal:
						device_id = "" if len(raw_names) < 3 else raw_names[2]
						af = {'peripheral': 'ClockOutput' + device_id}
						af.update({'type': 'out'})
						if af_id:
							af.update({'id': af_id})
						afs.append(af)

				if signal.startswith('I2C'):
					if name in ['scl', 'sda']:
						af = {'peripheral' : 'I2cMaster' + instance,
							  'name': name.capitalize()}
						if mode:
							af.update({'type': mode})
						if af_id:
							af.update({'id': af_id})
						afs.append(af)

				if signal.startswith('TIM'):
					for tname in raw_names[1:]:
						tinstance = raw_names[0].replace('TIM', '')
						nice_name = 'ExternalTrigger'
						output_type = 'in'
						if 'CH' in tname:
							nice_name = tname.replace('CH', 'Channel')
							output_type = None
						elif 'BKIN' in tname:
							nice_name = 'BreakIn'
						af = {'peripheral' : 'Timer' + tinstance,
							  'name': nice_name}
						if output_type:
							af.update({'type': output_type})
						if af_id:
							af.update({'id': af_id})
						afs.append(af)

				if signal.startswith('ADC'):
					if 'exti' not in name:
						af = {'peripheral' : 'Adc' + instance,
							  'name': name.replace('in', 'Channel').capitalize(),
							  'type': 'analog'}
						afs.append(af)

				if signal.startswith('SYS'):
					if 'mco' in name:
						af = {'peripheral' : signal.replace('SYS', '').replace('_', ''),
							  'type': 'out',
							  'id': '0'}
						afs.append(af)

				if signal.startswith('USB_OTG_FS') and raw_names[3] in ['DM', 'DP']:
					af = {'peripheral' : 'Usb',
						  'name': raw_names[3].capitalize()}
					if af_id:
						af.update({'id': af_id})
					else:
						af.update({'id': '10'})
					afs.append(af)

				if signal.startswith('USB_') and raw_names[1] in ['DM', 'DP']:
					af = {'peripheral': 'Usb',
						  'name': raw_names[1].capitalize()}
					if af_id:
						af.update({'id': af_id})
					# For the STM32F1 the USB pins aren't enabled like other
					# alternative functions, but by simply enabling the USB core.
					# else:
					# 	af.update({'id': '10'})
					afs.append(af)

				if signal.startswith('FSMC_') or signal.startswith('FMC_'):
					if not raw_names[1].startswith('DA'):
						af = {'peripheral' : 'Fsmc',
							  'name': raw_names[1].capitalize()}
						if af_id:
							af.update({'id': af_id})
						afs.append(af)

			# sort after key id and then add all without ids
			# this sorting only affect the way the debug information is displayed
			# in stm_writer the AFs are sorted again anyway
			sorted_afs = [a for a in afs if 'id' in a]
			sorted_afs.sort(key=lambda k: (int(k['id'].split(',')[0]), k['peripheral']))
			sorted_afs.extend([a for a in afs if 'id' not in a])

			for af in sorted_afs:
				af['gpio_port'] = gpio['port']
				af['gpio_id'] = gpio['id']
				gpio_afs.append(af)

		if 'CAN' in modules:
			modules.remove('CAN')

	def _modulesToString(self):
		string = ""
		char = self.modules[0][0:1]
		for module in self.modules:
			if not module.startswith(char):
				string += "\n"
			string += module + " \t"
			char = module[0][0:1]
		return string

	def _getDeviceDefine(self):
		if self.id.family not in stm32_defines:
			return None
		# get the defines for this device family
		familyDefines = stm32_defines[self.id.family]
		# get all defines for this device name
		devName = 'STM32F{}'.format(self.id.name)

		# Map STM32F7x8 -> STM32F7x7
		if self.id.family == 'f7' and devName[8] == '8':
			devName = devName[:8] + '7'

		deviceDefines = sorted([define for define in familyDefines if define.startswith(devName)])
		# if there is only one define thats the one
		if len(deviceDefines) == 1:
			return deviceDefines[0]

		# now we match for the size-id.
		devNameMatch = devName + 'x{}'.format(self.id.size_id.upper())
		for define in deviceDefines:
			if devNameMatch <= define:
				return define

		# now we match for the pin-id.
		devNameMatch = devName + '{}x'.format(self.id.pin_id.upper())
		for define in deviceDefines:
			if devNameMatch <= define:
				return define

		return None

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "STMDeviceReader({}, [\n{}])".format(os.path.basename(self.name), ",\n".join(map(str, self.properties)))

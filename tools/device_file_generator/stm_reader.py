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
from lxml import etree

import os, sys, math, glob, re
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger
# add python module device files to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))
from device_identifier import DeviceIdentifier

class STMDeviceReader(XMLDeviceReader):
	""" STMDeviceReader
	This STM specific part description file reader knows the structure and
	translates the data into a platform independent format.
	"""
	familyFile = None
	rootpath = None
	deviceDefines = {
		'f0': [
			'STM32F030x6',	# STM32F030x4, STM32F030x6 Devices (STM32F030xx microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)
			'STM32F030x8',	# STM32F030x8 Devices (STM32F030xx microcontrollers where the Flash memory is 64 Kbytes)
			'STM32F031x6',	# STM32F031x4, STM32F031x6 Devices (STM32F031xx microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)
			'STM32F038xx',	# STM32F038xx Devices (STM32F038xx microcontrollers where the Flash memory is 32 Kbytes)
			'STM32F042x6',	# STM32F042x4, STM32F042x6 Devices (STM32F042xx microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)
			'STM32F048x6',	# STM32F048xx Devices (STM32F042xx microcontrollers where the Flash memory is 32 Kbytes)
			'STM32F051x8',	# STM32F051x4, STM32F051x6, STM32F051x8 Devices (STM32F051xx microcontrollers where the Flash memory ranges between 16 and 64 Kbytes)
			'STM32F058xx',	# STM32F058xx Devices (STM32F058xx microcontrollers where the Flash memory is 64 Kbytes)
			'STM32F070x6',	# STM32F070x6 Devices (STM32F070x6 microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)
			'STM32F070xB',	# STM32F070xB Devices (STM32F070xB microcontrollers where the Flash memory ranges between 64 and 128 Kbytes)
			'STM32F071xB',	# STM32F071x8, STM32F071xB Devices (STM32F071xx microcontrollers where the Flash memory ranges between 64 and 128 Kbytes)
			'STM32F072xB',	# STM32F072x8, STM32F072xB Devices (STM32F072xx microcontrollers where the Flash memory ranges between 64 and 128 Kbytes)
			'STM32F078xx',	# STM32F078xx Devices (STM32F078xx microcontrollers where the Flash memory is 128 Kbytes)
			'STM32F030xC',	# STM32F030xC Devices (STM32F030xC microcontrollers where the Flash memory is 256 Kbytes)
			'STM32F091xC',	# STM32F091xC Devices (STM32F091xx microcontrollers where the Flash memory is 256 Kbytes)
			'STM32F098xx',	# STM32F098xx Devices (STM32F098xx microcontrollers where the Flash memory is 256 Kbytes)
		],
		'f1': [
			 'STM32F100xB',	# STM32F100C4, STM32F100R4, STM32F100C6, STM32F100R6, STM32F100C8, STM32F100R8, STM32F100V8, STM32F100CB, STM32F100RB and STM32F100VB
			 'STM32F100xE',	# STM32F100RC, STM32F100VC, STM32F100ZC, STM32F100RD, STM32F100VD, STM32F100ZD, STM32F100RE, STM32F100VE and STM32F100ZE
			 'STM32F101x6',	# STM32F101C4, STM32F101R4, STM32F101T4, STM32F101C6, STM32F101R6 and STM32F101T6 Devices
			 'STM32F101xB',	# STM32F101C8, STM32F101R8, STM32F101T8, STM32F101V8, STM32F101CB, STM32F101RB, STM32F101TB and STM32F101VB
			 'STM32F101xE',	# STM32F101RC, STM32F101VC, STM32F101ZC, STM32F101RD, STM32F101VD, STM32F101ZD, STM32F101RE, STM32F101VE and STM32F101ZE
			 'STM32F101xG',	# STM32F101RF, STM32F101VF, STM32F101ZF, STM32F101RG, STM32F101VG and STM32F101ZG
			 'STM32F102x6',	# STM32F102C4, STM32F102R4, STM32F102C6 and STM32F102R6
			 'STM32F102xB',	# STM32F102C8, STM32F102R8, STM32F102CB and STM32F102RB
			 'STM32F103x6',	# STM32F103C4, STM32F103R4, STM32F103T4, STM32F103C6, STM32F103R6 and STM32F103T6
			 'STM32F103xB',	# STM32F103C8, STM32F103R8, STM32F103T8, STM32F103V8, STM32F103CB, STM32F103RB, STM32F103TB and STM32F103VB
			 'STM32F103xE',	# STM32F103RC, STM32F103VC, STM32F103ZC, STM32F103RD, STM32F103VD, STM32F103ZD, STM32F103RE, STM32F103VE and STM32F103ZE
			 'STM32F103xG',	# STM32F103RF, STM32F103VF, STM32F103ZF, STM32F103RG, STM32F103VG and STM32F103ZG
			 'STM32F105xC',	# STM32F105R8, STM32F105V8, STM32F105RB, STM32F105VB, STM32F105RC and STM32F105VC
			 'STM32F107xC',	# STM32F107RB, STM32F107VB, STM32F107RC and STM32F107VC
		],
		'f2': [
			'STM32F205xx',
			'STM32F215xx',
			'STM32F207xx',
			'STM32F217xx',
		],
		'f3': [
			'STM32F301x8',	# STM32F301K6, STM32F301K8, STM32F301C6, STM32F301C8, STM32F301R6 and STM32F301R8 Devices
			'STM32F302x8',	# STM32F302K6, STM32F302K8, STM32F302C6, STM32F302C8, STM32F302R6 and STM32F302R8 Devices
			'STM32F302xC',	# STM32F302CB, STM32F302CC, STM32F302RB, STM32F302RC, STM32F302VB and STM32F302VC Devices
			'STM32F302xE',	# STM32F302CE, STM32F302RE, and STM32F302VE Devices
			'STM32F303x8',	# STM32F303K6, STM32F303K8, STM32F303C6, STM32F303C8, STM32F303R6 and STM32F303R8 Devices
			'STM32F303xC',	# STM32F303CB, STM32F303CC, STM32F303RB, STM32F303RC, STM32F303VB and STM32F303VC Devices
			'STM32F303xE',	# STM32F303RE, STM32F303VE and STM32F303ZE Devices
			'STM32F373xC',	# STM32F373C8, STM32F373CB, STM32F373CC, STM32F373R8, STM32F373RB, STM32F373RC, STM32F373V8, STM32F373VB and STM32F373VC Devices
			'STM32F334x8',	# STM32F334C4, STM32F334C6, STM32F334C8, STM32F334R4, STM32F334R6 and STM32F334R8 Devices
			'STM32F318xx',	# STM32F318K8, STM32F318C8: STM32F301x8 with regulator off: STM32F318xx Devices
			'STM32F328xx',	# STM32F328C8, STM32F328R8: STM32F334x8 with regulator off: STM32F328xx Devices
			'STM32F358xx',	# STM32F358CC, STM32F358RC, STM32F358VC: STM32F303xC with regulator off: STM32F358xx Devices
			'STM32F378xx',	# STM32F378CC, STM32F378RC, STM32F378VC: STM32F373xC with regulator off: STM32F378xx Devices
			'STM32F398xx',	# STM32F398CE, STM32F398RE, STM32F398VE: STM32F303xE with regulator off: STM32F398xx Devices
		],
		'f4': [
			'STM32F405xx',	# STM32F405RG, STM32F405VG and STM32F405ZG Devices
			'STM32F415xx',	# STM32F415RG, STM32F415VG and STM32F415ZG Devices
			'STM32F407xx',	# STM32F407VG, STM32F407VE, STM32F407ZG, STM32F407ZE, STM32F407IG and STM32F407IE Devices
			'STM32F417xx',	# STM32F417VG, STM32F417VE, STM32F417ZG, STM32F417ZE, STM32F417IG and STM32F417IE Devices
			'STM32F427xx',	# STM32F427VG, STM32F427VI, STM32F427ZG, STM32F427ZI, STM32F427IG and STM32F427II Devices
			'STM32F437xx',	# STM32F437VG, STM32F437VI, STM32F437ZG, STM32F437ZI, STM32F437IG and STM32F437II Devices
			'STM32F429xx',	# STM32F429VG, STM32F429VI, STM32F429ZG, STM32F429ZI, STM32F429BG, STM32F429BI, STM32F429NG, STM32F439NI, STM32F429IG and STM32F429II Devices
			'STM32F439xx',	# STM32F439VG, STM32F439VI, STM32F439ZG, STM32F439ZI, STM32F439BG, STM32F439BI, STM32F439NG, STM32F439NI, STM32F439IG and STM32F439II Devices
			'STM32F401xC',	# STM32F401CB, STM32F401CC, STM32F401RB, STM32F401RC, STM32F401VB and STM32F401VC Devices
			'STM32F401xE',	# STM32F401CD, STM32F401RD, STM32F401VD, STM32F401CE, STM32F401RE and STM32F401VE Devices
			'STM32F411xE',	#STM32F411CD, STM32F411RD, STM32F411VD, STM32F411CE, STM32F411RE and STM32F411VE Devices
		]
	}

	@staticmethod
	def getDevicesFromFamily(family, logger=None, rootpath=None):
		if rootpath is None:
			rootpath = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'STM_devices')
		STMDeviceReader.rootpath = rootpath

		STMDeviceReader.familyFile = XMLDeviceReader(os.path.join(rootpath, 'families.xml'), logger)
		families = STMDeviceReader.familyFile.query("//Family[@Name='{}']/SubFamily/Mcu/@RefName".format(family))
		logger.debug("STMDeviceReader: Found devices of family '{}': {}".format(family, ", ".join(families)))
		return families


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
		coreLut = {'m0': 'v6m', 'm3': 'v7m', 'm4': 'v7em'}
		core = self.query('//Core')[0].text.replace('ARM ', '').lower()
		self.addProperty('architecture', coreLut[core.replace('cortex-', '')])
		if core.endswith('m4'):
			core += 'f'
		self.addProperty('core', core)

		# flash and ram sizes
		# The <ram> and <flash> can occur multiple times.
		# they are "ordered" in the same way as the `(S-I-Z-E)` ids in the device combo name
		# we must first find out which index the current self.id.size_id has inside `(S-I-Z-E)`
		sizeIndex = 0

		matchString = "\(.(-.)*\)"
		match = re.search(matchString, comboDeviceName)
		if match:
			sizeArray = match.group(0)[1:-1].lower().split("-")
			sizeIndex = sizeArray.index(self.id.size_id)

		rams = self.query("//Ram")
		if len(rams) <= sizeIndex:
			sizeIndex = 0

		ram = int(rams[sizeIndex].text) * 1024
		flash = int(self.query("//Flash")[sizeIndex].text) * 1024
		self.addProperty('ram', ram)
		self.addProperty('flash', flash)

		# packaging
		package = self.query("//@Package")[0]
		self.addProperty('pin-count', re.findall('[0-9]+', package)[0])
		self.addProperty('package', re.findall('[A-Za-z\.]+', package)[0])

		# device header
		self.addProperty('header', 'stm32' + self.id.family + 'xx.h')

		# device defines
		define = self._getDeviceDefine()
		if define is None:
			logger.warn("STMDeviceReader: Define not found for device '{}'".format(self.id.string))
		else:
			self.addProperty('define', define)

		if self.id.family == 'f4':
			# required for our FreeRTOS
			self.addProperty('define', 'STM32F4XX')

		# if self.id.family == 'f3':
		# 	linkerscript = "%s_%s.ld" % ('stm32f3xx', dev.size_id)
		# else:
		# 	linkerscript = "%s_%s.ld" % ('stm32' + dev.family + 'xx', dev.size_id)
		# self.addProperty('linkerscript', linkerscript)

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
		self.log.debug("Available Modules are:\n" + self._modulesToString())

		for m in self.modules:
			if any(m.startswith(per) for per in ['TIM', 'UART', 'USART', 'ADC', 'DAC', 'CAN', 'SPI', 'I2C', 'OTG', 'DMA', 'USB', 'FSMC']):
				modules.append(m)

		invertMode = {'out': 'in', 'in': 'out', 'io': 'io'}
		nameToMode = {'rx': 'in', 'tx': 'out', 'cts': 'in', 'rts': 'out', 'ck': 'out',	# Uart
					 'miso': 'in', 'mosi': 'out', 'nss': 'io', 'sck': 'out',	# Spi
					 'scl': 'out', 'sda': 'io'}	# I2c

		# lets load additional information about the GPIO IP
		ip_file = self.query("//IP[@Name='GPIO']")[0].get('Version')
		ip_file = os.path.join(self.rootpath, 'IP', 'GPIO-' + ip_file + '_Modes.xml')
		self.gpioFile = XMLDeviceReader(ip_file, logger)

		pins = self.query("//Pin[@Type='I/O'][starts-with(@Name,'P')]")
		pins = sorted(pins, key=lambda p: [p.get('Name')[1:2], int(p.get('Name')[:4].split('-')[0].split('/')[0][2:])])

		for pin in pins:
			name = pin.get('Name')
			pinSignals = self.gpioFile.compactQuery("//GPIO_Pin[@Name='%s']/PinSignal/SpecificParameter[@Name='GPIO_AF']/.." % name)
			altFunctions = { a.get('Name') : a[0][0].text.replace('GPIO_AF', '')[:2].replace('_','') for a in pinSignals }
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
				instance = raw_names[0][-1]
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
					invertName = {'miso': 'somi', 'mosi': 'simo', 'nss': 'nss', 'sck': 'sck'}
					af.update({	'peripheral' : 'SpiSlave' + instance,
								'name': invertName[name].capitalize()})
					if mode:
						af.update({'type': invertMode[nameToMode[name]]})
					afs.append(af)

				if signal.startswith('CAN'):
					af = {'peripheral' : 'Can' + instance,
						  'name': name.capitalize()}
					if mode:
						af.update({'type': mode})
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

				if signal.startswith('OTG_FS') and raw_names[2] in ['DM', 'DP']:
					af = {'peripheral' : 'Usb',
						  'name': raw_names[2].capitalize()}
					if af_id:
						af.update({'id': af_id})
					else:
						af.update({'id': '10'})
					afs.append(af)

				# if signal.startswith('USB'):
				# 	af = {'peripheral' : 'Usb',
				# 		  'name': name.capitalize()}
				# 	if mode:
				# 		af.update({'type': mode})
				# 	if af_id:
				# 		af.update({'id': af_id})
				# 	afs.append(af)

				if signal.startswith('FSMC_'):
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
			sorted_afs.sort(key=lambda k: (int(k['id']), k['peripheral']))
			sorted_afs.extend([a for a in afs if 'id' not in a])

			for af in sorted_afs:
				af['gpio_port'] = gpio['port']
				af['gpio_id'] = gpio['id']
				gpio_afs.append(af)


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
		if self.id.family not in STMDeviceReader.deviceDefines:
			return None
		# get the defines for this device family
		familyDefines = STMDeviceReader.deviceDefines[self.id.family]
		# get all defines for this device name
		devName = 'STM32F{}'.format(self.id.name)
		deviceDefines = sorted([define for define in familyDefines if define.startswith(devName)])
		# if there is only one define thats the one
		if len(deviceDefines) == 1:
			return deviceDefines[0]

		# now we match for the size-id.
		devName += 'x{}'.format(self.id.size_id.upper())
		for define in deviceDefines:
			if devName <= define:
				return define

		return None

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "STMDeviceReader({}, [\n{}])".format(os.path.basename(self.name), ",\n".join(map(str, self.properties)))

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
import glob
from lxml import etree

import os, sys, math
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

	def __init__(self, file, logger=None):
		# we need to combile several xml files
		
		# The memory files are actually the best starting point, since they
		# are named most specifically after the device
		memoryFile = XMLDeviceReader(file, logger)
		memory = memoryFile.query("//Root")[0]
		# this name string contains all relevant information
		name = memory.get('name')
		# the device identifier parses it
		dev = DeviceIdentifier(name.lower())
		
		# now we have to search the correct peripheral configuration files
		# they use the same file for several size_ids, therefore we have to
		# filter them manually
		device = ''
		files = glob.glob(os.path.join(os.path.dirname(file), '..', name[:-1] + '*'))
		for deviceFile in files:
			fileName = os.path.basename(deviceFile).replace(name[:-1], '').replace('.xml', '')
			# Tx -> LQFP, Ux -> VFQFPN, Hx -> LFBGA, Yx -> WLCSP
			# we only take the QFP as reference
			if fileName[-2:] in ['Tx', 'Ux']:
				if fileName[0] != '(':
					if fileName[0] in [name[-1:], 'x']:
						device = deviceFile
						break
				else:
					# (S-I-Z-E)Tx
					sizes = fileName[1:-3].split('-')
					if name[-1:] in sizes:
						device = deviceFile
						break
		
		if device == '' and logger:
			logger.error("STMDeviceReader: Could not find device file for device '%s'" % dev.string)
			logger.error("STMDeviceReader: Possible files are: " + str([os.path.basename(f) for f in files]))
			return
		
		# this peripheral file is the actual, important file to work with
		XMLDeviceReader.__init__(self, device, logger)
		self.name = name
	
		# Some information about core and architecture can be found in the
		# propertyGroup.xml file
		propertyGroup = XMLDeviceReader(os.path.join(os.path.dirname(file), 'propertyGroups.xml'), self.log)
		architecture = propertyGroup.query("//groupEntry[@name='%s']/property[@name='arm_architecture']" % name)[0].get('value').lower()
		core = propertyGroup.query("//groupEntry[@name='%s']/property[@name='arm_core_type']" % name)[0].get('value').lower()
		defines = propertyGroup.query("//groupEntry[@name='%s']/cdefine" % name)
		for define in defines:
			cdef = define.get('name')
			if cdef.startswith('STM32'):
				self.properties['define'] = cdef
		
		self.properties['id'] = dev
		self.properties['mmcu'] = dev.string
		self.properties['core'] = core
		self.properties['architecture'] = architecture

		flash = memoryFile.query("//MemorySegment[@name='FLASH']")[0].get('size')
		ram = memoryFile.query("//MemorySegment[@name='RAM']")[0].get('size')
		# only the STM32F4 has Core Coupled Memory
		if dev.family == 'f4':
			data_sram = memoryFile.query("//MemorySegment[@name='DATA_SRAM']")[0].get('size')
		else:
			data_sram = '0'
		self.properties['flash'] = int(flash, 16)
		self.properties['ram'] = int(ram, 16) + int(data_sram, 16)

		self.properties['gpios'] = gpios = []
		self.properties['peripherals'] = peripherals = []
		self.properties['modules'] = modules = []

		self.modules = self.query("//IP/@InstanceName")
		self.log.debug("Available Modules are:\n" + self._modulesToString())

		for pin in self.query("//Pin[@Type='I/O']"):
			name = pin.get('Name')
			if '-' in name:
				name = name.split('-')[0]
			elif '/' in name:
				name = name.split('/')[0]
			
			gpio = {'port': name[1:2], 'id': name[2:], 'af': []}
			
			
			gpios.append(gpio)

	def _modulesToString(self):
		string = ""
		char = self.modules[0][0:1]
		for module in self.modules:
			if not module.startswith(char):
				string += "\n"
			string += module + " \t"
			char = module[0][0:1]
		return string

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "AVRDeviceReader(" + self.name + ")"

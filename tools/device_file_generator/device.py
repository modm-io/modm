# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------


from reader import XMLDeviceReader
from identifiers import Identifiers

import os, sys
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger
# add python module device files to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))
from device_identifier import DeviceIdentifier

class Device:
	""" Device
	Represents a device.
	"""

	def __init__(self, description_file=None, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		self.ids = None
		if description_file == None:
			self.ids = Identifiers()
			return
		
		# proper handling of Parsers
		if isinstance(description_file, XMLDeviceReader):
			self.ids = Identifiers(description_file.id)
			self.properties = list(description_file.properties)
		else:
			self.properties = list(description_file)
		
		if self.ids == None:
			self.ids = Identifiers()

		# if flash or ram is missing, it is a bad thing and unsupported
		if 'flash' not in self.properties:
			self.log.error("No FLASH found")
			return None
		if 'ram' not in self.properties:
			self.log.error("No RAM found")
			self.log.error("XPCC does not support Assembler-only programming!")
			return None
		# eeprom is optional on AVR and not available on ARM devices
		if 'eeprom' not in self.properties and 'avr' == self.id.platform:
			self.log.warn("No EEPROM found")

	def getMergedDevice(self, other):
		"""
		Merges the values of both devices and add a dictionary of differences
		"""
		assert isinstance(other, Device)
		self.log.info("Merging '%s'  with  '%s'" % (self.ids.string, other.ids.string))
		
		# update the ids in both
		self.ids.extend(other.ids)
		
		# go through all properties and merge each one
		for self_property in self.properties:
			for other_property in other.properties:
				if self_property.name == other_property.name:
					self_property = self_property.getMergedProperty(other_property)
		
		self.properties.sort(key=lambda k : k.name)

		return self;
	
	@property
	def id(self):
		return self.ids.intersection
	
	def getProperty(self, name):
		for prop in self.properties:
			if prop.name == name:
				return prop

		return None
	
	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return ("Device(\t%s,\n\n%s )\n" % (self.ids.string, \
			",\n\n".join([str(p) for p in self.properties]))) \
			.replace("\n", "\n\t")

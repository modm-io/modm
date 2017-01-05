#!/usr/bin/env pythonNone
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Niklas Hauser
# Copyright (c) 2016, Fabian Greif
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

from logger import Logger

from .reader import XMLDeviceReader
from .identifiers import Identifiers


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
		self.properties = []
		if description_file == None:
			self.ids = Identifiers(None, self.log)
			return

		# proper handling of Parsers
		if isinstance(description_file, XMLDeviceReader):
			self.properties = list(description_file.properties)
			self.log = description_file.log
			self.ids = Identifiers(description_file.id, self.log)
		else:
			self.properties = list(description_file)

		if self.ids == None:
			self.ids = Identifiers(None, self.log)

		# if flash or ram is missing, it is a bad thing and unsupported
		if self.getProperty('flash') == None:
			self.log.error("No FLASH found")
			return None
		if self.getProperty('ram') == None:
			self.log.error("No RAM found")
			self.log.error("MODM does not support Assembler-only programming!")
			return None
		# eeprom is optional on AVR and not available on ARM devices
		if (self.getProperty('eeprom') == None) and ('avr' == self.id.platform):
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

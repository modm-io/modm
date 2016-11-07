#!/usr/bin/env pythonNone
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, 2015, Niklas Hauser
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

from .property import Property

class XMLDeviceReader:
	""" DeviceReader
	Base class for all readers for handling the opening and reading of XML files etc...
	"""

	def __init__(self, path, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger

		self.file = path
		self.tree = self._openDeviceXML(self.file)
		self.id = DeviceIdentifier()
		self.properties = []

	def _openDeviceXML(self, filename):
		self.log.debug("XMLDeviceReader: Opening XML file '%s'" % os.path.basename(self.file))
		file = open(filename, 'r').read()
		file = re.sub(' xmlns="[^"]+"', '', file, count=1)
		xmltree = None
		try:
			# parse the xml-file
			xmltree = etree.fromstring(file)
		except:
			self.log.error("XMLDeviceReader: Failure to open XML file!")
		return xmltree

	def queryTree(self, query):
		"""
		This tries to apply the query to the device tree and returns either
		- an array of element nodes,
		- an array of strings or
		- None, if the query failed.
		"""
		response = None
		try:
			response = self.tree.xpath(query)
		except:
			self.log.error("XMLDeviceReader: Query failed for '%s'" % str(query))

		return response

	def query(self, query):
		"""
		This wraps the queryTree and returns an (empty) array. 
		"""
		self.log.debug("XMLDeviceReader: Querying for '%s'" % str(query))
		response = self.queryTree(query)

		if response != None:
			if len(response) == 0:
				self.log.warn("XMLDeviceReader: No results found for '%s'" % str(query))
			return response

		return []

	def addProperty(self, name, value):
		self.properties.append(Property(self.id, name, value, self.log))

	def compactQuery(self, query):
		result = self.queryTree(query)
		if result != None:
			result = list(set(result))
			result.sort()
		return result


	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "XMLDeviceReader({}, [\n{}])".format(os.path.basename(self.file), ",\n".join(map(str, self.properties)))

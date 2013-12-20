# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

from lxml import etree

import os, sys, re
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger
# add python module device files to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))
from device_identifier import DeviceIdentifier

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
		self.properties = {'instances': [], 'id': DeviceIdentifier()}

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
		This tries to apply the query to the device tree and returns eiher
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
	
	def compactQuery(self, query):
		result = self.queryTree(query)
		if result != None:
			result = list(set(result))
			result.sort()
		return result


	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "XMLDeviceReader(%s)" % os.path.basename(self.file)

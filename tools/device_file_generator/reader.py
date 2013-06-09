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

from lxml import etree

import os, sys
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger
# add python module device files to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))
from device_string import DeviceString

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
		self.properties = {'instances': [], 'id': DeviceString()}

	def _openDeviceXML(self, filename):
		self.log.debug("XMLDeviceReader: Opening XML file '" + os.path.basename(self.file) + "'")
		xmltree = None
		try:
			# parse the xml-file
			xmltree = etree.parse(filename).getroot()
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
			self.log.error("XMLDeviceReader: Query failed for '" + str(query) + "'")
		
		return response
	
	def query(self, query):
		"""
		This wraps the queryTree and returns an (empty) array. 
		"""
		self.log.debug("XMLDeviceReader: Querying for '" + str(query) + "'")
		response = self.queryTree(query)
		
		if response != None:
			if len(response) == 0:
				self.log.info("XMLDeviceReader: No results found for '" + dict['id'].string + "'")
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
		return "XMLDeviceReader(" + os.path.basename(self.file) + ")"

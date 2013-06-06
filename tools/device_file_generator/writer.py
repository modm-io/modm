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

class XMLDeviceWriter:
	""" DeviceWriter
	Base class for all writers for handling the opening and writing of XML files etc...
	"""

	def __init__(self, device, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		self.file = None
		self.device = device
		self.root = etree.Element('device')
		
		props = self.device.properties
		dev = props['device']
		
		self.setAttributes(self.root, {'platform': dev.platform, 'family': dev.family, 'name': "|".join(self.device.getDeviceNames())})
		

	def writeXMLTree(self, tree):
		pass

	def _openNewXML(self, filename):
		self.log.debug("Opening new XML file: " + os.path.basename(self.file))
		try:
			# parse the xml-file
			xmltree = et.parse(filename).getroot()
		except OSError as e:
			raise ParserException(e)
		except (xml.parsers.expat.ExpatError, xml.etree.ElementTree.ParseError) as e:
			raise ParserException("while parsing xml-file '%s': %s" % (filename, e))
		return xmltree

	def setAttributes(self, node, dict):
		for key in dict:
			self.setAttribute(node, key, dict[key])
			
	def setAttribute(self, node, key, value):
		node.set(key, str(value))
	
	def addChild(self, node, name):
		return etree.SubElement(node, name)
	
	def setValue(self, node, value):
		node.text = value

	def writeToString(self):
		return etree.tostring(self.root, pretty_print=True)

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "XMLDeviceWriter(" + os.path.basename(self.file) + ")"

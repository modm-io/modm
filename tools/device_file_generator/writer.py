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
		props = self.device.properties
		dev = props['device']
		
		self.tree = XMLElement('rca')
		self.tree.setAttribute('version', '1.0')
		
		self.root = self.tree.addChild('device')
		self.root.setAttributes({'platform': dev.platform, 'family': dev.family, 'name': "|".join(self.device.getDeviceNames())})
		

	def writeToFile(self, file):
		if os.path.exists(file):
			self.log.warn("XMLDeviceWriter: Overwriting file '" + os.path.basename(file) + "'")
		else:
			self.log.info("XMLDeviceWriter: New XML file: '" + os.path.basename(file) + "'")
		self.file = file 
		
		with open(self.file, 'w') as device_file:
			device_file.write(self.toString())
	
	def writeToFolder(self, folder, name):
		abspath = os.path.abspath(folder)
		if os.path.isdir(abspath):
			filename = os.path.join(folder, name)
			self.writeToFile(filename)
		else:
			self.log.error("XMLDeviceWriter: Path is not a folder! " + folder)

	def toString(self):
		return etree.tostring(self.tree.root,
							encoding="UTF-8",
							pretty_print=True,
							xml_declaration=True,
							doctype='<!DOCTYPE rca SYSTEM "../devicefile.dtd">')

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "XMLDeviceWriter(" + os.path.basename(self.file) + ")"


class XMLElement:
	""" XMLElement
	Helper class to wrap and extend the methods of etree.Element and etree.SubElement
	"""
	
	def __init__(self, tag=None):
		self.root = None
		if tag != None:
			self.root = etree.Element(tag)
	
	def setAttributes(self, dict):
		for key in dict:
			self.setAttribute(key, dict[key])
			
	def setAttribute(self, key, value):
		self.root.set(key, str(value))
	
	def addChild(self, name):
		element = XMLElement()
		element.root = etree.SubElement(self.root, name)
		return element
	
	def setValue(self, value):
		self.root.text = str(value)
	
	def toString(self):
		return etree.tostring(self.root, pretty_print=True)
	
	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "XMLElement(\n" + self.toString() + ")"


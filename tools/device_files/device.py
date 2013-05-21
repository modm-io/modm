#!/usr/bin/env python
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

import re
import os
import xml.etree.ElementTree as et
import xml.parsers.expat
from parser_exception import ParserException
from device_element import DeviceElementBase, DeviceString
from driver import Driver

class Device:
	""" Device
	Represents a device target for xpcc.
	This can be a microcontroller, a pc or a doxygen "fake" target.
	"""

	def __init__(self, xml_file):
		self.node = self._openDeviceXML(xml_file)

		self.platform = self.node.get('platform')
		self.family = self.node.get('family')
		self.names = self.node.get('name').split('|')

		self.properties = []
		self.drivers = []

		for c in self.node:
			if c.tag in ['flash', 'ram', 'pin-count', 'define']:
				self.properties.append(Property(self, c))
			elif c.tag in ['driver']:
				self.drivers.append(Driver(self, c))
			else:
				raise ParserException("Parse Error: unknown tag (%s)" % (c.tag))

	def _openDeviceXML(self, filename):
		try:
			# parse the xml-file
			xmltree = et.parse(filename).getroot()
		except OSError as e:
			raise ParserException(e)
		except (xml.parsers.expat.ExpatError, xml.etree.ElementTree.ParseError) as e:
			raise ParserException("while parsing xml-file '%s': %s" % (filename, e))
		"""
		TODO: Validate!!
				try:
					import lxml.etree		# for validating
			
					# validate against the embedded DTD file
					try:
						parser = lxml.etree.XMLParser(dtd_validation=True)
						dummy = lxml.etree.parse(filename, parser)
					except lxml.etree.XMLSyntaxError as e:
						raise ParserException("Validation error in '%s': %s" % (filename, e))
					else:
						logging.debug("Validation OK!")
				except ImportError as e:
					logging.warning("Warning: couldn't load 'lxml' module. No validation done!")
		"""
		# since we have totally "validated" our xml file (see above) we can
		# expect that all required elements exist...
		return xmltree[0]

	def getProperty(self, prop_type, device_string):
		"""
		Can be used to inquire flash size, ram size, pin-count or defines
		Always returns as list if a valid device string is handed to function.
		"""
		s = DeviceString(device_string)
		if s.valid == False:
			return None
		values = []
		for prop in self.properties:
			if prop.type == prop_type:
				if prop.appliesTo(s):
					values.append(prop.value)
		return values

	def getBuildList(self, peripheral_path, device_string, target_path=None):
		"""
		target_path: normally the absolute path to the architecture/platform_device_string.generated directory
		"""
		s = DeviceString(device_string)
		if s.valid == False:
			return False
		pin_count = self.getProperty('pin-count', device_string)
		if len(pin_count) != 1:
			raise ParserException("Parse Error: none definite pin_count: %s" % (pin_count))
		else:
			pin_count = int(pin_count[0])
		build = []
		for driver in self.drivers:
			build_list = driver.getBuildList(peripheral_path, device_string, pin_count)
			if build_list != None:
				build = build + build_list # => unite lists to make a list to rule them all
		# patch build list for templates to include target substitution variable
		old_build = build
		build = []
		for f in old_build:
			if len(f) == 3: # is template file
				f[2] = dict(f[2].items() + s.getTargetDict().items()) # merge substitutions
			build.append(f)
		# patch build list to absolute pahts
		if target_path != None:
			old_build = build
			build = []
			for f in old_build:
				f[0] = os.path.join(peripheral_path, f[0])
				f[1] = os.path.join(target_path, f[1])
				build.append(f)
		return build

	def __str__(self):
		s  = "Platform: " + self.platform + "\n"
		s += "Family: " + self.family + "\n"
		s += "Names: "
		for name in self.names:
			s += name + ", "
		return s

class Property(DeviceElementBase):
	""" Property
	Represents a device property, i.e.
	flash, ram, pin-count, define
	"""

	def __init__(self, device, node):
		DeviceElementBase.__init__(self, device, node)
		self.value = node.text
		self._checkValue()
		# print "New Property of type: %s and value: %s" % (self.type, self.value)

	def _checkValue(self):
		if self.type in ['flash', 'ram', 'pin-count']:
			if not self.value.isdigit():
				raise ParserException("Invalid content of '%s' property: '%s' is not an integer." % (self.type, self.value))
			else:
				self.value = int(self.value)
		elif self.type in ['define']:
			if not re.match("^[0-9A-Z_]*$", self.value):
				raise ParserException("Value of '%s' properties needs to be UPPER_UNDERSCORE_STYLE (found: '%s')" % (self.type, self.value))


if __name__ == "__main__":
	"""
	Some test code
	"""
	dev = Device("../../src/xpcc/architecture/platform/xml/stm32f40.xml")
	a_dir = os.path.abspath("../../src/xpcc/architecture/")
	build = dev.getBuildList(os.path.join(a_dir, 'platform'), 'stm32f407vg', os.path.join(a_dir, 'platform_stm32f407vg.generated'))
	for f in build:
		if len(f) == 2:
			print "static:   %s => %s" % (f[0], f[1])
		elif len(f) == 3:
			print "template: %s => %s" % (f[0], f[1])
			print "substitutions: %s" % f[2]
	print "dev.getProperty('flash', 'stm32f407vg'): %s" % dev.getProperty('flash', 'stm32f407vg')
	print "dev.getProperty('flash', 'stm32f407ve'): %s" % dev.getProperty('flash', 'stm32f407ve')

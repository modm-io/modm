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

import re, os, sys
import xml.etree.ElementTree as et
import xml.parsers.expat
from string import Template
from parser_exception import ParserException
from device_element import DeviceElementBase
from device_string import DeviceString

# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

class DeviceFile:
	""" DeviceFile
	Represents a device target for xpcc.
	This can be a microcontroller, a pc or a doxygen "fake" target.
	"""

	PROPERTY_TAGS = ['flash', 'ram', 'eeprom', 'pin-count', 'define', 'header', 'linkerscript']

	def __init__(self, xml_file, logger=None):
		node = self._openDeviceXML(xml_file)

		if logger == None:
			self.log = Logger()
		else:
			self.log = logger

		self.platform = node.get('platform')
		self.family = node.get('family')
		self.names = node.get('name').split('|')

		self.properties = []
		self.drivers = []

		for c in node:
			if c.tag in self.PROPERTY_TAGS:
				self.properties.append(Property(self, c))
			elif c.tag in ['driver']:
				self.drivers.append(Driver(self, c))
			else:
				raise ParserException("Parse Error: unknown tag (%s)" % (c.tag))
		self._checkDeviceFileData(xml_file)

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

	def _checkDeviceFileData(self, xml_file):
		"""
		This checks if there are any problems with the device file.
		It also considers the different requirements of the various platforms.
		Warnings/Errors are logged when a problem is discovered. An information
		is logged when there is something that probably could be improved.
		This method won't throw any errors because any error that prevents the
		device to be built will be discovered later. It is intended to notify
		users if device files aren't completely correct instead.
		"""
		# Print some Information
		self.log.info("Using Xml Device File '%s'" % os.path.basename(xml_file))
		self.log.info("Found %s Properties:" % len(self.properties))
		# Check Properties
		prop_count = {}
		for prop in self.properties:
			if prop.type in prop_count:
				prop_count[prop.type] += 1
			else:
				prop_count[prop.type] = 1
		self.log.info("Found %s Drivers" % len(self.drivers))
		# Checks for all Platforms

		# Checks for STM32 Platform
		if self.platform == 'stm32':
			families = ['f0', 'f1', 'f2', 'f3', 'f4']
			if self.family not in families:
				self.log.error("Unknown family '%s' for platform %s."
				" Valid families for this platform are: %s" %
				(self.family, self.platform, families))
		elif self.platform == 'avr':
			families = ['at90','attiny', 'atmega', 'atxmega']
			if self.family not in families:
				self.log.error("Unknown family '%s' for platform %s."
				" Valid families for this platform are: %s" %
				(self.family, self.platform, families))
			pass
		else:
			self.log.error("Unknown platform '%s'" % self.platform)



##-------------  Methods used by the SCons Platform Tools ---------------------
	def getProperties(self, device_string):
		"""
		Returns a dictionary containing:
		'flash', 'ram', 'pin-count' and 'defines'
		that are specific to the device_string.

		This is used by the Scons Platform Tools. Think before editing.
		TODO: defines, flash and ram may depend on pin-count....
		"""
		s = DeviceString(device_string)
		if s.valid == False:
			return None
		props = {}
		props['flash'] = self.getProperty('flash', device_string, True)[0]
		props['ram'] = self.getProperty('ram', device_string, True)[0]
		props['eeprom'] = self.getProperty('eeprom', device_string, True, 0)[0]
		props['pin-count'] = self.getProperty('pin-count', device_string, True)[0]
		props['linkerscript'] = self.getProperty('linkerscript', device_string, True, "")[0]
		props['defines'] = self.getProperty('define', device_string)
		props['headers'] = self.getProperty('header', device_string)
		props.update(s.getTargetDict())

		#Check Some Properties
		if self.platform == 'stm32':
			if props['eeprom'] != 0:
				self.log.warn("On platform stm32 there is no eeprom.")
		if self.platform == 'stm32':
			if props['eeprom'] != 0:
				self.log.warn("On platform stm32 there is no eeprom.")

		return props

	def getDriverList(self, device_string, platform_path):
		"""
		This function uses data gathered from the device file to generate
		a list fo drivers that need to be built.
		The platform_path is needed in order to return absolute paths.
		Every driver is represented by a dictionary that contains the following
		keys:
		'name':
		'type':
		'driver_file': contains the path to the drivers xml file
		               if it exists, else the value is None
		'path': path to the driver files
		'substitutions': substitution values that are derived from the device
		                 file, will be appended by those introduced by the
		                 driver file
		'instances': list of instances that will be created of this driver
		Please note: all paths are relative to the platform_path.
		"""
		# Check Device string
		s = DeviceString(device_string)
		if s.valid == False:
			return None
		# Check Pin Count
		pin_count = self.getProperty('pin-count', device_string)
		if pin_count == None or len(pin_count) <= 0:
			pin_count = 10000
		elif len(pin_count) == 1:
			pin_count = pin_count[0]
		else:
			pin_count = 0
		# Loop Through Drivers
		drivers = []
		for d in self.drivers:
			if d.appliesTo(s, pin_count):
				substitutions = s.getTargetDict()
				substitutions.update(self.getSubstitutions())
				drivers.append(d.toDict(platform_path, substitutions))
		return drivers

##-----------------------------------------------------------------------------
	def getProperty(self, prop_type, device_string, require_singelton=False, default=None):
		"""
		Can be used to inquire flash size, ram size, pin-count or defines
		Always returns as list if a valid device string is handed to function.
		Set require_singelton if you need exactly one value to be returned.
		"""
		s = DeviceString(device_string)
		if s.valid == False:
			return None
		if prop_type == 'define':
			values = {}
			for prop in self.properties:
				if prop.type == prop_type:
					if prop.appliesTo(s):
						values[prop.value] = prop.content
		else:
			values = []
			for prop in self.properties:
				if prop.type == prop_type:
					if prop.appliesTo(s):
						values.append(prop.value)
		if require_singelton:
			if len(values) > 1:
				raise ParserException("There can only be one %s for %s. %s found: %s" \
					% (prop_type, device_string, len(values), values))
			elif len(values) < 1:
				if default == None:
					raise ParserException("There needs to be at least one %s for %s." \
						% (prop_type, device_string))
				else:
					values.append(default)
		return values

	def getSubstitutions(self):
		"""
		Generates a Dictionary containing
		Substitutitons for this device
		At the moment that is nothing
		"""
		dic = {}
		return dic

	def __str__(self):
		s  = "Platform: " + self.platform + "\n"
		s += "Family: " + self.family + "\n"
		s += "Names: "
		for name in self.names:
			s += name + ", "
		return s

##------------- A Driver Node contains Driver and Property Nodes --------------
class Driver(DeviceElementBase):

	def __init__(self, device, node):
		DeviceElementBase.__init__(self, device, node)
		self.type = node.get('type') # this overwrite is somewhat unfortunate
		self.name = node.get('name')
		self.instances = node.get('instances')
		if self.instances != None:
			self.instances = self.instances.split(',')
		# Calculate driver path relative to architecture
		if self.type == 'core':
			self.path = 'core'
		else:
			self.path = os.path.join('peripheral', self.type)
		self.path = os.path.join(self.path, os.sep.join(self.name.split('/')))
		self.substitutions = self._getDriverSubstitutions(node)

	def toDict(self, platform_path, substitutions):
		"""
		This is used to package information about a driver extracted from
		a device file into a dictionary.
		Corresponds to the fromDict Method of the DriverFile class
		in driver.py.
		Remember to update if you change anything!
		"""
		dic = {}
		dic['name'] = self.name
		dic['type'] = self.type
		dic['driver_file'] = self.getDriverFile(platform_path)
		dic['path'] = self.path
		dic['substitutions'] = substitutions
		dic['substitutions'].update(self.substitutions) # own substitutions overwrite
		dic['instances'] = self.instances
		return dic

	def getDriverFile(self, platform_path):
		"""
		Returns None if file does not exist
		"""
		f = os.path.join(self.path, self.name.split('/')[-1:][0] + '.xml')
		if not os.path.isfile(os.path.join(platform_path, f)):
			return None
		else:
			return f

	def _getDriverSubstitutions(self, node):
		"""
		Returns a dict containing substitution values
		that are specific to this driver.
		"""
		# If Substitutions Dict does not exist => create
		substitutions = {}
		# Probably the less interesting stuff, but maybe ther will be other
		# more usefull stuff that can be added here
		substitutions['driver-name'] = self.name
		substitutions['driver-type'] = self.type
		if len(node) <= 0: # if the node is childless
			return substitutions
		# Now this is were it gets interesting:
		# parsing the inner nodes of the driver node recursively:
		substitutions = dict(self._nodeToDict(node).items() + substitutions.items())
		# If this is the gpio driver parse gpio nodes in order to
		# derive nibbles and ports
		if self.type == 'gpio':
			self._gpioCreateNibblePort(substitutions)
		self.log.debug("Found substitutions: " + str(substitutions))
		return substitutions

	def _nodeToDict(self, node):
		"""
		attribute of the node are turnded into key/value pairs
		child nodes are added to a list which is the value
		of the child node name + 's' key.
		Example:
		TODO..
		"""
		# Fist add attributes
		dic = dict(node.items())
		# Now add children
		for c in node:
			child_name = c.tag + 's'
			if child_name not in dic:
				dic[child_name] = [] # create child list
			dic[child_name].append(self._nodeToDict(c))
		return dic

	def _gpioCreateNibblePort(self, substitutions):
		# a nibble is 4 bit, an octet is 8bit
		# a port is either 8 bit for AVR or 16 bit for ARM
		gpios = substitutions['gpios']
		if gpios != None:
			# unique port list
			ports = {v['port'] for v in gpios}
			portArrays = []
			for port in ports:
				ids = [0] * 16
				for gpio in gpios:
					if gpio['port'] == port:
						ids[int(gpio['id'])] = 1
				# lets find some nibbles
				nibbles = [1] * 4
				for nibble in range(4):
					for id in range(4):
						if ids[nibble*4+id] == 0:
							nibbles[nibble] = 0
				# lets find some ports
				octets = [1] * 2
				for octet in range(2):
					for nibble in range(2):
						if nibbles[octet*2+nibble] == 0:
							octets[octet] = 0

				portArrays.append( {'port': port, 'nibbles': nibbles, 'octets': octets} )

			# reduce the arrays and create two of them
			nibbleSub = []
			octetSub = []

			for port in portArrays:
				nibbles = []
				for nibble in range(4):
					if port['nibbles'][nibble] == 1:
						nibbles.append(nibble)
				octets = []
				for octet in range(2):
					if port['octets'][octet] == 1:
						octets.append(octet)
				if len(nibbles) > 0:
					nibbleSub.append( {'port': port['port'], 'position': nibbles} )
				if len(octets) > 0:
					octetSub.append( {'port': port['port'], 'position': octets} )

			substitutions['nibbles'] = nibbleSub
			substitutions['octets'] = octetSub



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
		if self.type in ['flash', 'ram', 'eeprom', 'pin-count']:
			if not self.value.isdigit():
				raise ParserException("Invalid content of '%s' property: '%s' is not an integer." % (self.type, self.value))
			else:
				self.value = int(self.value)
		elif self.type in ['define']:
			# Separate Value (= Name of Define) and Content
			if self.value.find('=') > 0:
				self.content = self.value[self.value.find('=')+1:].strip()
				self.value = self.value[:self.value.find('=')].strip()
			else:
				self.content = '1'

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
#
# This file contains the code that identifies drivers.
#
#

import os, sys
import xml.etree.ElementTree as et
import xml.parsers.expat

from parser_exception import ParserException
from device_identifier import DeviceIdentifier
from device_element import DeviceElementBase
from parameters import ParameterDB

if sys.version_info[0] >= 3:
	basestring = (str, bytes)

# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger


class DriverFile:

	def __init__(self, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger

		self.name = None
		self.type = None
		self.filename = None
		self.path = None
		# Parameters from the XML Device File
		self.device_parameters = []
		self.substitutions = None
		self.instances = None
		self.properties = None
		# Parameters that can be set by the user (e.g. in the project.cfg file)
		self.user_parameters = []

	@classmethod
	def fromDict(self, dic, user_parameters, logger=None):
		"""
		This is used to create a DriverFile instance from information about
		a driver extracted from a device file and packaged into a dictionary.
		Corresponds to the toDict Method of the Driver class
		in device.py.
		Remember to update if you change anything!
		"""
		d = DriverFile(logger)
		d.name = dic['name']
		d.type = dic['type']
		d.filename = dic['driver_file']
		d.path = dic['path']
		# Parameters from the XML Device File
		d.device_parameters = dic['parameters']
		d.substitutions = dic['substitutions']
		d.instances = dic['instances']
		d.properties = dic['properties']
		# generate ParameterDB from dict
		param_db = ParameterDB.fromDictionary(user_parameters, logger)
		d.user_parameters = param_db.getParametersForDriver(d)

		# Debug Output
		# d.log.debug("DriverFile.fromDict(): %s" % dic)
		# d.log.debug("d.device_core : %s" % d.device_core)
		return d

	def getBuildList(self, platform_path, device_id, source_file_extentions = ['.cpp', '.c', '.sx', '.S', '.h', '.hpp']):
		"""
		The data is put into a list of the format:
		[ ['static_file_name', 'static_output_file_name'],
		['template_src_file','template_target_file',{'Dict': 'a', 'of': 'b', 'defines': 's'}]]
		Note: all file paths are relative to the platform path.
		"""
		# Turn Device String into Device Identifier
		if isinstance(device_id, basestring):
			device_id = DeviceIdentifier(device_id)
		# Initialize Return List
		build = []
		# Check if there is a driver file and open
		if self.filename != None:
			try:
				f = os.path.join(platform_path, self.filename)
				xmltree = et.parse(f).getroot()
			except OSError as e:
				raise ParserException(e)
			except (xml.parsers.expat.ExpatError, xml.etree.ElementTree.ParseError) as e:
				raise ParserException("while parsing xml-file '%s': %s" % (f, e))
			# TODO: Validate
			node = xmltree[0]
			# Check if name and type match
			# If they do not, why should everything else be correct?....
			if node.get('type') != self.type:
				raise ParserException("Error: Driver: Type in device file (%s) and type in driver file (%s) do not match. File: %s"
				% (self.type, node.get('type'), f))
			if node.get('name') != self.name:
				raise ParserException("Error: Driver: Name in device file (%s) and name in driver file (%s) do not match. File: %s"
				% (self.name, node.get('name'), f))
			# Do we only have one default instance?
			if self.instances == None:
				self._parseDriverXml(device_id, node, 'default', build, {})
			else: # handle several or at least one specific instance
				# Initialize Internal Target File Dictionary
				targets = {}
				for instance in self.instances:
					self._parseDriverXml(device_id, node, instance, build, targets)
		else: # if no xml driver file exists, just add all files that look like source files
			# Query all files in directory
			dir = os.path.join(platform_path, self.path)
			if not os.path.exists(dir):
				self.log.warn("'%s' implementation for target '%s' does not exist!" % (self.path, device_id.string))
				return build
			for source_file in os.listdir(dir):
				# Detect Static Source and Header Files
				if os.path.splitext(source_file)[1] in source_file_extentions:
					source_file = self._makeRelativeToPlatform(source_file)
					build.append([source_file, source_file])
				# Detect Template Files
				elif os.path.splitext(source_file)[1] == '.in':
					template = self._makeRelativeToPlatform(source_file)
					output = self._makeRelativeToPlatform(source_file[:-3])
					substitutions = self.substitutions
					substitutions.update({'output': os.path.split(output)[1]})
					build.append([template, output, self.substitutions])
		return build

	def _parseDriverXml(self, device_id, driver_node, instance_id, build_list, targets):
		# First parse parameters:
		for node in driver_node:
			if node.tag == 'parameter':
				p = Parameter(node, self.log)
				# 1.) search for user parameters
				value = p.getValueFromUserParameterDict(self.user_parameters, instance_id)
				# 2.) if none are found search for parameters in driver file
				if value == None:
					value = p.getValueFromParameterDict(self.device_parameters, instance_id)
				# 3.) else use the default value
				if value == None:
					value = p.default
				# 4.) add value found to the substitution dict
				p.addValueToSubstitutions(self.substitutions, value)

			elif node.tag not in ['static', 'template']:
				f = DeviceElementBase(self, node)
				if f.appliesTo(device_id, self.properties):
					self.log.debug("Found driver file substitution '%s' with value '%s'. In '%s/%s' driver xml" % (node.tag, node.text, self.type, self.name))
					self.substitutions.update({node.tag: node.text})

		# Then parse needed static/template files
		for node in driver_node:
			# Skip PArameters:
			if node.tag == 'parameter':
				continue

			# Check if instance id fits:
			if node.get('instances') != None and instance_id != None:
				if instance_id not in node.get('instances').split(','):
					continue

			if node.text == None or len(node.text) <= 0:
				self.log.error("Empty '%s' node in '%s/%s' driver xml." % (node.tag, self.type, self.name))
				raise ParserException("Error: Empty '%s' node in '%s/%s' driver xml." % (node.tag, self.type, self.name))

			f = DeviceElementBase(self, node)
			if not f.appliesTo(device_id, self.properties):
				self.log.info("Tag '%s' does not apply to target '%s'. In '%s/%s' driver xml." % (node.tag, device_id.string, self.type, self.name))
			else:
				file_name = node.text.strip()

				if node.tag == 'static':
					if file_name not in build_list: # if it has not been added before
						static = file_name
						output = node.get('out')
						if output == None or len(output) <= 0: # if no out attribute was found
							output = static
						static = self._makeRelativeToPlatform(static)
						output = self._makeRelativeToPlatform(output)
						if self._checkTarget([static, output], targets):
							build_list.append([static, output])# => add static file

				elif node.tag == 'template':
					template = file_name
					output = node.get('out')
					if output == None or len(output) <= 0: # if no out attribute was found
						output = template
						if output.endswith('.in'):
							output = output[:-3]
					else: # replace '{{id}}' with id
						output = output.replace('{{id}}', instance_id.lower())
					template = self._makeRelativeToPlatform(template)
					output = self._makeRelativeToPlatform(output)
					if instance_id.isdigit():
						sub_instance_id = int(instance_id)
					else:
						sub_instance_id = instance_id
					substitutions = dict(self.substitutions)
					if 'parameters' in substitutions:
						substitutions['parameters'] = dict(substitutions['parameters'])
					# substitutions['parameters'] = dict(substitutions['parameters'])
					if node.get('instances') != None:
						substitutions.update({'id': sub_instance_id})
					substitutions.update({'output': os.path.split(output)[1]})
					# self.log.debug("%s->%s: substitutions: %s" % (template, output, substitutions))
					template_file = [template, output, substitutions]
					if self._checkTarget(template_file, targets):
						build_list.append(template_file) # always append template files since they will get a different id

	def _checkTarget(self, build, targets):
		"""
		Checks if this target file has already been added to the build list.
		Returns false if the target has already bean added which means
		that you should not add it again.
		If a different build was specified before for the same target name
		a warning is generated.
		"""
		if build[1] not in targets.keys():
			targets[build[1]] = build
			return True
		else:
			# This does not work as intended. -Niklas
			if targets[build[1]] != build:
				pass
				# self.log.warn("Two different builds for '%s' specified! "
				# 	"'%s' will be built while '%s' will be ignored." % (build[1], targets[build[1]], build))
			return False

	def _makeRelativeToPlatform(self, file_name):
		"""
		All file names are relative to the xml device
		description file.
		This function makes them relative to the platform directory.
		That makes a lot of sense because the generated files are going
		to retain the same structure in this context (this sentence
		admittedly does not make as much sense...)
		"""
		# Check if we need to go up directories (if path starts with '../')
		go_up = 0
		if file_name.startswith('./'): # do not know why you would want to do this
			file_name = file_name[2:]  # but whatever => go ahead (btw. this code was written at 1:00 am)
		while file_name.startswith('../'):
			go_up = go_up + 1
			file_name = file_name[3:]
		path_length = len(self.path.split(os.sep))
		if go_up > path_length:
			raise ParserException("Illegal file name: %s. Must not leave the platform directory." % (file_name))
		path = os.sep.join(self.path.split(os.sep)[:(path_length-go_up)])
		file_name = os.path.join(path, file_name)
		return file_name


class Parameter:
	"""
	This represents a Parameter that can be specified in a
	XML Driver File
	"""
	TYPES = ['int', 'bool', 'enum', 'string']

	def __init__(self, node, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger

		self.name = node.get('name')
		self.type = node.get('type')
		self.values = node.get('values')
		self.instances = node.get('instances')
		self.max = node.get('max')
		self.min = node.get('min')
		self.default = node.text

		self._evaluate()

	def getValueFromParameterDict(self, dic, instance_id):
		if self.name in dic:
			if dic[self.name]['instances'] == None:
				return dic[self.name]['value']
			elif instance_id != None and instance_id != 'default':
				if instance_id in dic[self.name]['instances']:
					return dic[self.name]['value']
		return None

	def getValueFromUserParameterDict(self, dic, instance_id):
		for param in dic:
			if param['name'] == self.name:
				if instance_id != None and instance_id != 'default':
					if str(instance_id) == str(param['instance']):
						return param['value']
				elif instance_id == 'default':
					return param['value']
		return None

	def addValueToSubstitutions(self, sub_dict, value):
		value = self.evaluateValue(value)
		if value != None:
			if 'parameters' not in sub_dict:
				sub_dict['parameters'] = {}
			sub_dict['parameters'][self.name] = value

	def evaluateValue(self, value):
		if self.type == 'int':
			if isinstance(value, basestring):
				value = value.strip(' \t\n\r')
			if isinstance(value, basestring) and not value.isdigit():
				self.log.error("Invalid value '%s' for %s. The value is not a number!"
					% (value, self.name))
				return None
			value = int(value)
			if self.max != None and value > self.max:
				self.log.error("Invalid value '%s' for %s. %s > Max(=%s)"
					% (value, self.name, value, self.max))
				return None
			if self.min != None and value < self.min:
				self.log.error("Invalid value '%s' for %s. %s < Min(=%s)"
					% (value, self.name, value, self.min))
				return None
			return value
		elif self.type == 'bool':
			if isinstance(value, basestring):
				value = value.strip(' \t\n\r')
			if isinstance(value, bool):
				return value
			if value.lower() == 'true' or value == '1':
				return True
			elif value.lower() == 'false' or value == '0':
				return False
			self.log.error("Invalid value '%s' for %s." % (value, self.name))
			return None
		elif self.type == 'enum':
			if isinstance(value, basestring):
				value = value.strip(' \t\n\r')
			if value in self.values:
				return value
			else:
				self.log.error("Invalid value '%s' for %s. Valid values are: %s"
					% (value, self.name, self.values))
				return None
		elif self.type == 'string':
			return value
		return None

	def _evaluate(self):
		if self.name == None:
			self.log.error("<parameter> is missing the name attribute.")
			return False
		if self.type not in self.TYPES:
			self.log.error("Parameter '%s' has invalid type '%s'."% (self.name, self.type))
			self.log.info("Valid types are: %s" % str(self.TYPES))
			return False
		if self.type == 'int':
			if self.max != None:
				self.max = int(self.max)
			if self.min != None:
				self.min = int(self.min)
		elif self.type == 'enum':
			if self.values == None:
				self.log.error("Parameter '%s': enums need a 'values' attribute that specifies valid values." % self.name)
				return False
			else:
				self.values = self.values.split(';')
		elif self.type == 'string':
			pass
		if self.instances != None:
			self.instances = self.instances.split('|')
		if self.default != None:
			self.default = self.evaluateValue(self.default)
			if self.default == None:
				self.log.error("Parameter '%s': invalid default parameter (%s)." % (self.name, self.default))
				return False
		return True

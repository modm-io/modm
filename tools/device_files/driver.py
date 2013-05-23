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
#
# This file contains the code that identifies drivers.
#
#

import os
import xml.etree.ElementTree as et
import xml.parsers.expat

from parser_exception import ParserException
from device_element import DeviceElementBase, DeviceString

class Driver(DeviceElementBase):
	""" Peripheral
	Represents a driver needed by the device, i.e.
	peripheral or core driver.

	Use like this:
	1.) Create from XML by calling the constructor with a valid node
	2.) Call the gather data function to parse the driver file
	3.) Now you can access the substitutions, templateFiles and staticFiles
	    members
	"""


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
		self.substitutions = None
		self._getDriverSubstitutions() # load substitutions haha
		# Static Variables
		self.source_file_extentions = ['.cpp', '.sx', '.c', '.ld', '.s'] # FIXME: are there other files that need to be considered?
		# print "New Driver of type: " + self.type

	def getBuildList(self, peripheral_path, device_string, pin_count):
		"""
		Searches for the xml file belonging to this driver, opens it
		and extracts all data needed for creating this driver for the
		specific device_string
		The data is put into a list of the format:
		[ ['static_file_name', 'static_output_file_name'],
		['template_src_file','template_target_file',{'Dict': 'a', 'of': 'b', 'defines': 's'}]]
		Please Note:
		It is the Device Classes Responibility to add Device specific substitution
		values like device family, name, pincount etc.
		"""
		# Initialize Return List
		build = []
		# Check if this driver "applies"
		if not self.appliesTo(device_string, pin_count):
			return None
		# Build Path to xml 
		f = os.path.join(peripheral_path, self.path, self.name.split('/')[-1:][0] + '.xml')
		# Check if file exists and open
		xmltree = None
		if os.path.isfile(f):
			try:
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
				self._parseDriverXml(node, 'default', build)
			else: # handle several or at least one specific instance
				for instance in self.instances:
					self._parseDriverXml(node, instance, build)
		else: # if no xml driver file exists, just add all files that look like source files
			# Query all files in directory
			for source_file in os.listdir(os.path.join(peripheral_path, self.path)):
				if os.path.splitext(source_file)[1].lower() in self.source_file_extentions:
					source_file = self._makeAbsoluteToPlatform(source_file)
					build.append([source_file, source_file])
		return build

	def _parseDriverXml(self, driver_node, instance_id, build_list):
		for node in driver_node:
			# Check if instance id fits:
			if node.get('instances') != None and instance_id != None and instance_id != 'default':
				if instance_id not in node.get('instances').split(','):
					continue
			if node.text == None or len(node.text) <= 0:
				raise ParserException("Error: Empty %s node in %s/%s driver xml." % (node.tag, self.type, self.name))
			if node.tag == 'static':
				if node.text not in build_list: # if it has not been added before
					static = node.text
					output = node.get('out')
					if output == None or len(output) <= 0: # if no out attribute was found
						output = static
					static = self._makeAbsoluteToPlatform(static)
					output = self._makeAbsoluteToPlatform(output)
					build_list.append([static, output])# => add static file
			elif node.tag == 'template':
				template = node.text
				output = node.get('out')
				if output == None or len(output) <= 0: # if no out attribute was found
					output = template
					if output.endswith('.in'):
						output = output[:-3]
				else: # replace '{{id}}' with id
					output = output.replace('{{id}}', instance_id)
				template = self._makeAbsoluteToPlatform(template)
				output = self._makeAbsoluteToPlatform(output)
				substitutions = dict({'id': instance_id}.items() + self._getDriverSubstitutions().items())
				template_file = [template, output, substitutions]
				build_list.append(template_file) # always append template files since they will get a different id

	def _makeAbsoluteToPlatform(self, file_name):
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

	def _getDriverSubstitutions(self):
		"""
		Returns a dict containing substitution values
		that are specific to this driver.
		Don't know if all of these are necessary but whatever.
		This uses lazy evaluation (because the author wanted to try that
		out after reading about it in More Effective C++) thus
		once the result was calculated it will be stored and returned
		every time this method is called
		"""
		if self.substitutions != None:
			return self.substitutions
		# If Substitutions Dict does not exist => create
		self.substitutions = {}
		# Probably the less interesting stuff, but maybe ther will be other
		# more usefull stuff that can be added here
		self.substitutions['driver-name'] = self.name
		self.substitutions['driver-type'] = self.type
		if len(self.node) <= 0: # if the node is childless
			return self.substitutions
		# Now this is were it gets interesting:
		# parsing the inner nodes of the driver node recursively:
		self.substitutions = dict(self._NodeToDict(self.node).items() + self.substitutions.items())
		return self.substitutions


	def _NodeToDict(self, node):
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
			dic[child_name].append(self._NodeToDict(c))
		return dic


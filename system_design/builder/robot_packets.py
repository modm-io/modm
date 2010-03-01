#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
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
# 
# $Id$
# -----------------------------------------------------------------------------

import os
import builder_base

BUILTIN = [	'int8_t', 'int16_t', 'int32_t',
			'uint8_t', 'uint16_t', 'uint32_t',
			'char', 'float' ]

# -----------------------------------------------------------------------------
def filter_type(value):
	if value in BUILTIN:
		return value
	return value.title().replace(' ', '')

def filter_variable(value):
	value = value.title().replace(' ', '')
	value = value[0].lower() + value[1:]
	return value

def filter_subtype(value):
	""" value needs to be parser.structure.SubType """
	type = filter_type(value.type.name)
	variable = filter_variable(value.name)
	if value.type.is_array:
		return "%s %s[%s]" % (type, variable, value.type.count)
	else:
		return "%s %s" % (type, variable)

def filter_constructor(classtype, default=True):
	if default:
		return "%s()" % filter_type(classtype.name)
	else:
		parameter = []
		for item in classtype.iter():
			if item.type.is_array:
				raise builder.BuilderException("Array handling is incomplete " \
						"right now! Could not generate code for %s" % item)
			else:
				type = filter_type(item.type.name)
				name = filter_variable(item.name)
				
				parameter.append("%s %s" % (type, name))
		
		return "%s(%s)" % (filter_type(classtype.name), ", ".join(parameter))

def filter_initialization_list(classtype, default=True):
	initList = []
	for item in classtype.iter():
		if item.type.is_array:
			raise builder.BuilderException("Array handling is incomplete " \
					"right now! Could not generate code for %s" % item)
		else:
			type = filter_type(item.type.name)
			name = filter_variable(item.name)
			
			if default:
				initList.append("%s()" % name)
			else:
				initList.append("%s(%s)" % (name, name))
	
	return ", ".join(initList)

# -----------------------------------------------------------------------------
class TypeBuilder(builder_base.Builder):
	
	VERSION = "$Id$"
	
	def setup(self, optparser):
		optparser.add_option(
				"--source_path",
				dest = "source_path",
				default = None,
				help = "Output path for the source file")
		optparser.add_option(
				"--header_path",
				dest = "header_path",
				default = None,
				help = "Output path for the header file")
	
	def generate(self):
		# check the commandline options
		if self.options.outpath:
			source_path = self.options.outpath
			header_path = self.options.outpath
		elif self.options.source_path and self.options.header_path:
			source_path = self.options.source_path
			header_path = self.options.header_path
		else:
			raise builder.BuilderException("You need to provide an output path!")
		
		filter = {
			'cpp.variable': filter_variable,
			'cpp.type': filter_type,
			'cpp.subtype': filter_subtype,
			'cpp.constructor': filter_constructor,
			'cpp.initialization_list': filter_initialization_list
		}
		
		template_header = self.template('templates/robot_packets.hpp.tpl', filter=filter)
		template_source = self.template('templates/robot_packets.cpp.tpl', filter=filter)
		
		substitutions = {
			'components': self.tree.components,
			'actions': self.tree.components.actions,
			'events': self.tree.components.events,
			'types': self.tree.types
		}
		
		file = os.path.join(header_path, 'packets.hpp')
		self.write(file, template_header.render(substitutions))
		
		file = os.path.join(source_path, 'packets.cpp')
		self.write(file, template_source.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	TypeBuilder().generate()

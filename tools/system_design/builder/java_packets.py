#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2010-2011, 2013, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2015, Kevin Läufer
# Copyright (c) 2015, Niklas Hauser
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import builder_base
import filter.java as filter

# -----------------------------------------------------------------------------
def fromBufferMethod(element):
	if isinstance(element, filter.Primitive):
		type = str(element.type)
	else:
		type = str(element.subtype.name)
	if type in filter.PRIMITIVES:
		if type == "Bool":
			return "new Bool(buffer.get() != 0)"
		elif (type == "char"):
			return "new Char((char)buffer.get())"
		else:
			if (filter.PRIMITIVES[type].mask == None):
				return "new %s(buffer.get%s())" % ( 
						filter.PRIMITIVES[type].name,
						filter.PRIMITIVES[type].accessor)
			else:
				return "new %s(buffer.get%s()&%s)" % (
						filter.PRIMITIVES[type].name,
						filter.PRIMITIVES[type].accessor,
						filter.PRIMITIVES[type].mask,)
			
	else:
		return "%s.fromBuffer(buffer)" % (filter.typeName(type))

def toBufferMethod(element, name=None):
	if isinstance(element, filter.Primitive):
		type = str(element.type)
	else:
		type = str(element.subtype.name)
	if name == None:
		name = filter.variableName(element.name)
	if type in filter.PRIMITIVES:
		if type == "Bool":
			return "buffer.put((byte) (%s ? 1 : 0))" % name
		else:
			return "buffer.put%s((%s) %s)" % \
						(filter.PRIMITIVES[type].accessor,
						 filter.PRIMITIVES[type].equivalent,
						 name)
	else:
		return "%s.toBuffer(buffer)" % name

def toBufferMethodStructAccess(element, name=None):
	if isinstance(element, filter.Primitive):
		type = str(element.type)
	else:
		type = str(element.subtype.name)
	if name == None:
		name = filter.variableName(element.name)
	if type in filter.PRIMITIVES:
		return "new %s(%s).toBuffer(buffer)" % \
						(filter.PRIMITIVES[type].name,
						 name)
	else:
		return "%s.toBuffer(buffer)" % name

# -----------------------------------------------------------------------------
class JavaPacketsBuilder(builder_base.Builder):
	"""
	Generate the whole packageset. The Output is a Java Class named Packages.java,
	which contains as subclasses all the Packages.
	
	A common call would be like:
	$python2 java_packets.py  --outpath source/rca/robot --package rca.robot robot.xml;
	"""
	
	
	VERSION = "0.1"
	
	def setup(self, optparser):
		optparser.add_option(
				"--package",
				dest = "package",
				default = '',
				help = "name of package")
				
	def generate(self):
		# check the commandline options
		if not self.options.outpath:
			raise builder_base.BuilderException("You need to provide an output path!")
		
		javaFilter = {
			'enumElement': filter.enumElement,
			'enumElementStrong': filter.typeName,
			'typeName': filter.typeName,
			'typeObjectName': filter.typeObjectName,
			'variableName': filter.variableName,
			'fromBufferMethod': fromBufferMethod,
			'toBufferMethod': toBufferMethod,
			'toBufferMethodStructAccess': toBufferMethodStructAccess,
		}
		template = self.template('templates/java_packets.tpl',
								filter = javaFilter)
		
		# Bool has a special status because its primitive but user generated
		# and the only not numerical type
		packets = self.tree.types
		packets.remove('Bool')
		
		primitives = filter.PRIMITIVES.values()
		primitives.sort()
		
		substitutions = {
			'package' : self.options.package,
			'packets': packets,
			'primitives': primitives,
		}
		
		file = os.path.join(self.options.outpath, 'Packets.java')
		self.write(file, template.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	JavaPacketsBuilder().run()

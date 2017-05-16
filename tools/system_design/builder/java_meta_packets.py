#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2010-2011, 2013, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2015, Kevin Läufer
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
			'typeName': filter.typeName,
			'typeObjectName': filter.typeObjectName,
			'typeJavaObjectName': filter.typeJavaObjectName,
			'variableName': filter.variableName,
			'inStringDescription': filter.inStringDescription,
		}
		template = self.template('templates/java_meta_packets.tpl',
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
		
		file = os.path.join(self.options.outpath, 'MetaPackets.java')
		self.write(file, template.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	JavaPacketsBuilder().run()

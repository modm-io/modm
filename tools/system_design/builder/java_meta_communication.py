#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2010-2011, 2013, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2015, Kevin Läufer
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2017, Michael Thies
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
class JavaCommunicationBuilder(builder_base.Builder):
	"""
	Generate the whole communication tree. The Output is a class named
	MetaCommunication. It Provides a list of all concrete Components.
	Each Component provides it's name, description and a list of Actions.
	Each Action provides it's name, description, type of parameter and
	a generic call mechanism.
	
	This is intended to be used in a generic user interface.
	
	A common call would be like:
	$python3 java_meta_communication.py  --outpath source/rca/robot --package rca.robot robot.xml;
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
			'variableName': filter.variableName,
			'inStringDescription': filter.inStringDescription,
		}
		template = self.template('templates/java_meta_communication.tpl',
								filter = javaFilter)
		
		# Bool has a special status because its primitive but user generated
		# and the only not numerical type
		components = self.tree.components
		
		primitives = sorted(filter.PRIMITIVES.values())
		
		substitutions = {
			'package' : self.options.package,
			'components': components,
		}
		
		file = os.path.join(self.options.outpath, 'MetaCommunication.java')
		self.write(file, template.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	JavaCommunicationBuilder().run()

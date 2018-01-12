#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2010-2011, 2013, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2014, Martin Rosekeit
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

class JavaIdentifierBuilder(builder_base.Builder):
	"""
	Generate the Identifiers. The Output is a Java Class named Identifier.java,
	which contains as subclasses three enums called Component, Action and Event.
	Each contains a get by id method. Events have a possibility to create a
	Packet from bytebuffer.
	
	A common call would be like:
	$ python3 java_identifier.py  --outpath source/rca/robot --package rca.robot robot.xml;
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
			'typeObjectName': filter.typeObjectName,
		}
		
		template = self.template('templates/java_identifier.tpl', filter=javaFilter)
		
		substitutions = {
			'domains' : self.tree.domains,
			'package' : self.options.package,
			'components': self.tree.components,
			'actions': self.tree.components.actions,
			'events': self.tree.events,
		}
		
		file = os.path.join(self.options.outpath, 'Identifier.java')
		self.write(file, template.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	JavaIdentifierBuilder().run()

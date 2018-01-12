#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2010-2011, 2013, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2014, Martin Rosekeit
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

class JavaGeneratorBuilder(builder_base.Builder):
	"""
	Generate one Static method which is able to retrieve a Package from bytebuffer in
	a typesafe manner based on xml communication definitions.
	
	A common call would be like:
	$python3 java_generator.py --outpath source/rca/robot --package rca.robot robot.xml;
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
		
		template = self.template('templates/java_generator.tpl', filter=javaFilter)
		
		substitutions = {
			'package' : self.options.package,
			'domains':	self.tree.domains,
			'components': self.tree.components,
			'actions': self.tree.components.actions,
			'events': self.tree.events,
		}
		
		file = os.path.join(self.options.outpath, 'Generator.java')
		self.write(file, template.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	JavaGeneratorBuilder().run()

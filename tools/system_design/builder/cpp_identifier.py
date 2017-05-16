#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2012, 2016, Sascha Schade
# Copyright (c) 2014, Martin Rosekeit
# Copyright (c) 2015, Georgi Grinshpun
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
import filter.cpp as filter

class IdentifierBuilder(builder_base.Builder):
	
	VERSION = "0.1"

	def setup(self, optparser):
		optparser.add_option(
				"--namespace",
				dest = "namespace",
				default = "robot",
				help = "Namespace of the generated identifiers.")

	def generate(self):
		# check the commandline options
		if not self.options.outpath:
			raise builder_base.BuilderException("You need to provide an output path!")

		if self.options.namespace:
			namespace = self.options.namespace
		else:
			raise builder_base.BuilderException("You need to provide a namespace!")

		cppFilter = {
			'enumElement': filter.enumElement,
			'enumElementStrong': filter.typeName,
			'enumValue': filter.toHexValue,
		}
		
		template = self.template('templates/robot_identifier.tpl', filter=cppFilter)
			
		components = []
		for component in self.tree.components.iter(abstract=False):
			components.append(component.flattened())
		
		substitutions = {
			'domains' : self.tree.domains,
			'containers' : self.tree.containers,
			'components': components,
			'actions': self.tree.components.actions,
			'events': self.tree.events,
			'namespace': namespace
		}
					
		if os.path.splitext(self.options.outpath)[1] == '':
			file = os.path.join(self.options.outpath, 'identifier.hpp')
		else:
			file = self.options.outpath
		self.write(file, template.render(substitutions) + "\n")

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	IdentifierBuilder().run()

#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2010-2011, 2013-2014, Georgi Grinshpun
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
import filter.cpp as filter

# -----------------------------------------------------------------------------
def filter_lower(value):
	return value.lower().replace(" ", "_")

# -----------------------------------------------------------------------------
class CppTaskCallerBuilder(builder_base.Builder):
	"""
	For each Action in Communication generate Caller-Classes. They wrap
	Actions calls and waiting for the Responses supporting the Asynchronous
	Task interface xpcc::CommunicatableTask.
	"""


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
			'camelcase': filter_lower,
			'camelCase': filter.variableName,
			'CamelCase': filter.typeName,
			'CAMELCASE': filter.enumElement,
		}
		template = self.template('templates/cpp_caller.tpl',
								filter = cppFilter)

		# Bool has a special status because its primitive but user generated
		# and the only not numerical type
		components = self.tree.components

		substitutions = {
			'components': components,
			'events': self.tree.events,
			'namespace': namespace
		}

		file = os.path.join(self.options.outpath, 'caller.hpp')
		self.write(file, template.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	CppTaskCallerBuilder().run()

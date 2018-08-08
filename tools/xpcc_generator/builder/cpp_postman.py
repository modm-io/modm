#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2012, 2014, 2016, Sascha Schade
# Copyright (c) 2015, Niklas Hauser
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
import filter.cpp as filter

# -----------------------------------------------------------------------------
def filter_lower(value):
	return value.lower().replace(" ", "_")

# -----------------------------------------------------------------------------
class PostmanBuilder(builder_base.Builder):

	VERSION = "0.1"

	def setup(self, optparser):
		optparser.add_option(
				"--container",
				dest = "container",
				default = None,
				help = "name of the container in the XML file")
		optparser.add_option(
				"--namespace",
				dest = "namespace",
				default = "robot",
				help = "Namespace of the generated identifiers.")

	def generate(self):
		# check the commandline options
		if not self.options.outpath:
			raise builder_base.BuilderException("You need to provide an output path!")
		if not self.options.container or self.options.container not in self.tree.containers:
			raise builder_base.BuilderException("Please specify a valid container!")

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

		container = self.tree.containers[self.options.container]

		components = []
		for component in container.components:
			components.append(component.flattened())

		# resumable function information
		resumableActions = 0;
		resumableActionsWithPayload = 0;
		for component in components:
			for action in component.actions:
				if action.call == "resumable":
					resumableActions += 1
					if action.parameterType is not None:
						resumableActionsWithPayload += 1

		substitutions = {
			'resumables': resumableActions,
			'resumablePayloads': resumableActionsWithPayload,
			'components': components,
			'events': self.tree.events,
			'container': container,
			'eventSubscriptions': container.subscriptions,
			'namespace': namespace
		}

		file = os.path.join(self.options.outpath, 'postman.hpp')
		self.write(file, self.template('templates/postman.hpp.tpl', filter=cppFilter).render(substitutions) + "\n")

		file = os.path.join(self.options.outpath, 'postman.cpp')
		self.write(file, self.template('templates/postman.cpp.tpl', filter=cppFilter).render(substitutions) + "\n")

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	PostmanBuilder().run()

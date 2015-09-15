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
	$python java_meta_communication.py  --outpath source/rca/robot --package rca.robot robot.xml;
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
		
		primitives = filter.PRIMITIVES.values()
		primitives.sort()
		
		substitutions = {
			'package' : self.options.package,
			'components': components,
		}
		
		file = os.path.join(self.options.outpath, 'MetaCommunication.java')
		self.write(file, template.render(substitutions))

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	JavaCommunicationBuilder().run()

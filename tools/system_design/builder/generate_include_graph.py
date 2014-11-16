#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2014, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

# This script takes the path to a xpcc `comunication xml` file
# and generated a graphviz file that represents all it's includes.
# You can use this tool in order to generate .dot files like this:
# `./generate_include_graph.py ../../../../season/communication/robot.xml --dtdpath ../xml/dtd --outpath . `
# Use something like `dot robot_include_graph.dot -Tpdf -o test.pdf`
# to generate pdfs from dot files.
#
# author: eKiwi <electron.kiwi@gmail.com>

import os, re
import builder_base

# -----------------------------------------------------------------------------
class IncludePathBuilder(builder_base.Builder):

	VERSION = '0.1'

	def generate(self):
		# check the commandline options
		if self.options.outpath:
			outpath = self.options.outpath
		else:
			raise builder_base.BuilderException("You need to provide an output path!")

		template = self.template('templates/include_graph.dot.tpl')
		substitutions = {'edges': self.generate_xml_include_vertices()}

		filename = os.path.basename(os.path.abspath(self.xmlfile))
		file = os.path.join(outpath, filename[:-4] + '_include_graph.dot')
		self.write(file, template.render(substitutions) + "\n")


	# The following code is straight from the scons/site_tools/system_design.py
	# file. Maybe there is a better way of sharing code.
	def find_includes(self, file, include_path):
		""" Find include directives in an XML file """
		includeExpression = re.compile(r'<include>(\S+)</include>', re.M)

		files = []
		line_count = 0
		for line in open(file).readlines():
			line_count = line_count + 1
			match = includeExpression.search(line)
			if match:
				filename = match.group(1)
				relative_to_file = os.path.join(os.path.dirname(os.path.abspath(file)), filename)
				relative_to_include_path = os.path.join(include_path, filename)
				# 1.) include file name can be absolut
				if os.path.isabs(filename):
					files.append(filename)
				# 2.) it could be a path relative to the files path
				#     this works just like #include "{filename}" in C/C++
				elif os.path.isfile(relative_to_file):
					files.append(relative_to_file)
				# 3.) it could be a path relative to the include path
				elif os.path.isfile(relative_to_include_path):
					files.append(relative_to_include_path)
				# 4.) Error!
				else:
					raise builder_base.BuilderException(
						"Could not find include file '%s' in '%s:%s'"
						% (filename, file, line_count))
		return files

	def generate_xml_include_vertices(self):
		""" Generates the dependency graph edges for the xml file """
		path, filename = os.path.split(os.path.abspath(self.xmlfile))

		max_stack_size = 40 # some precaution, because we are not detecting dependency loops
		stack = [filename]
		edges = []

		while stack:
			nextFile = stack.pop()
			files = self.find_includes(os.path.join(path, nextFile), path)
			for file in files:
				edges.append((os.path.basename(nextFile), os.path.basename(file)))
				if len(stack) < max_stack_size:
					stack.append(file)
				else:
					raise builder_base.BuilderException("Too many recoursions. You might have an include loop.")

		return edges

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	IncludePathBuilder().run()

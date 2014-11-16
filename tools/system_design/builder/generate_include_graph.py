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

import os, re, sys
import builder_base

# This is neccessary in order to use a common find_include_file method
sys.path = [os.path.join(os.path.dirname(__file__), '..')] + sys.path
from xmlparser.parser import Parser
from xmlparser.parser_exception import ParserException

# -----------------------------------------------------------------------------
class IncludePathBuilder(builder_base.Builder):

	VERSION = '0.1'

	def setup(self, optparser):
		optparser.add_option(
				"--label_path",
				dest = "label_path",
				default = None,
				help = "the path the node labels will be relative to")

	def generate(self):
		# check the commandline options
		if self.options.outpath:
			outpath = self.options.outpath
		else:
			raise builder_base.BuilderException("You need to provide an output path!")

		path, filename = os.path.split(os.path.abspath(self.xmlfile))

		if self.options.label_path:
			label_path = os.path.abspath(self.options.label_path)
		else:
			label_path = path

		template = self.template('templates/include_graph.dot.tpl')
		edges = self.generate_xml_include_vertices(path, filename, label_path)
		substitutions = {'edges': edges, 'label_path': label_path}

		filename = os.path.basename(os.path.abspath(self.xmlfile))
		file = os.path.join(outpath, filename[:-4] + '_include_graph.dot')
		self.write(file, template.render(substitutions) + "\n")

	def generate_xml_include_vertices(self, path, filename, label_path):
		""" Generates the dependency graph edges for the xml file """
		max_stack_size = 40 # some precaution, because we are not detecting dependency loops
		stack = [filename]
		edges = []

		while stack:
			nextFile = stack.pop()
			files = self.find_includes(os.path.join(path, nextFile), path)
			for file in files:
				a = os.path.relpath(os.path.join(path, nextFile), label_path)
				b = os.path.relpath(file, label_path)
				if (a, b) not in edges:
					edges.append((a, b))
				if len(stack) < max_stack_size:
					stack.append(file)
				else:
					raise builder_base.BuilderException("Too many recoursions. You might have an include loop.")

		return edges


	def find_includes(self, file, include_path):
		""" Find include directives in an XML file """
		includeExpression = re.compile(r'<include>(\S+)</include>', re.M)

		files = []
		line_count = 0
		for line in open(file).readlines():
			line_count = line_count + 1
			match = includeExpression.search(line)
			if match:
				try:
					filename = Parser.find_include_file(
						match.group(1),
						os.path.abspath(file),
						self.include_paths,
						str(line_count))
				except ParserException as e:
					raise builder_base.BuilderException(e.message)
				files.append(filename)
		return files

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	IncludePathBuilder().run()

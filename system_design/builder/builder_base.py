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
# 
# $Id$
# -----------------------------------------------------------------------------

import time
import codecs		# utf-8 file support
import sys
import os
import jinja2		# template engine
import textwrap
from optparse import OptionParser

sys.path = [os.path.join(os.path.dirname(__file__), '..')] + sys.path
from xmlparser.parser import Parser


class BuilderException(Exception):
	pass


class Builder(object):
	"""
	Base class for any builder-class
	
	Attributes:
	self.xmlpath	--	path to the xml-file
	self.xmlfile	--	xml-filename
	self.tree		--	evaluated xml tree 
	self.time		--	string containing the current date and time
	self.options	--	evaluated commandline arguments (see OptionParser)
	
	To access the elements from the xml tree use for example:
	self.tree.components
	self.tree.components.actions			--	all actions
	self.tree.components["driver"].actions	--	only actions form the
												component 'driver'
	self.tree.components.publish
	self.tree.components.subscribe
	self.tree.boards
	self.tree.boards["PC"].components		--	only components form the
												board 'PC'
	self.tree.types
	self.tree.events
	self.tree.events
	
	You have to specify a VERSION attribute.
	
	"""
	def __init__(self):
		optparser = OptionParser(
				usage   = "%prog [options] XML_FILE",
				version = "%prog version: " + self.VERSION )
		
		optparser.add_option(
				"-o", "--outpath",
				dest = "outpath",
				default = None,
				help = "Output path [optional]")
		
		optparser.add_option(
				"-t", "--template_path",
				dest = "templatePath",
				default = None,
				help = "Template path [optional]")
		
		self.setup(optparser)
		
		(self.options, args) = optparser.parse_args()
		
		if len(args) != 1:
			raise BuilderException("XML_FILE: You have to specify a valid xml-file")
			exit(1)
		
		self.xmlfile = args[0]
		self.xmlpath = os.path.dirname(os.path.abspath(self.xmlfile))
		
		parser = Parser(self.xmlfile)
		parser.parse()
		
		self.tree = parser
		self.globals = {
			'time': time.strftime("%d %b %Y, %H:%M:%S", time.localtime()),
		}
		
		if self.options.templatePath == None:
			self.templatePath = "templates/"
		else:
			self.templatePath = self.options.templatePath
	
	def generate(self):
		""" Generate the requested files """
		raise BuilderException("You can not use this class directly!")
	
	def setup(self, optparser):
		"""
		Setup the builder. Can be overwritten to customise the behavior.
		
		Keywords arguments:
		optparser		--	OptionParser instance. Use add_option for
							additional commandline arguments.
		"""
		pass
	
	def write(self, filename, data):
		"""
		Write data utf-8 decoded to a file.
		
		Contents of the file will be overwritten.
		"""
		# create the path if it doesn't exists
		dir = os.path.dirname(filename)
		if not os.path.isdir(dir):
			os.mkdir(dir)
		
		# write data
		file = codecs.open(filename, 'w', 'utf8')
		file.write(data)
		file.close()
	
	def read(self, filename):
		"""
		Read a utf-8 decoded file.
		
		Provided for convenience.
		"""
		return codecs.open(filename, 'r', 'utf8').read()
	
	def template(self, filename, filter=None):
		""" Open a template file
		
		Uses the jinja2 template engine. The following additional filters
		are included:
		xpcc.wordwrap(with)		--	like the original filter, but with correct
									handling of newlines
		xpcc.indent(level)		--	indent every line with \a level tabs
		
		Keyword arguments:
		filename	--	Template file
		filter		--	dict with additional filters (see the section 
						'Custom Filters' in the Jinja2 documentation for details)
		
		Example:
		template = builder.template(file)
		
		output = template.render(dict)
		builder.write(outputfile, output)
		
		"""
		def filter_wordwrap(value, width=79):
			return '\n\n'.join([textwrap.fill(str, width) for str in value.split('\n\n')])

		def filter_indent(value, level=0):
			return ('\n' + '\t' * level).join(value.split('\n'))
		
		path = os.path.dirname(filename)
		name = os.path.basename(filename)
		
		if not os.path.isabs(filename):
			relpath = os.path.dirname(os.path.abspath(__file__))
			path = os.path.join(relpath, path)
		
		environment = jinja2.Environment(loader = jinja2.FileSystemLoader(path))
		environment.filters['xpcc.wordwrap'] = filter_wordwrap
		environment.filters['xpcc.indent'] = filter_indent
		if filter:
			environment.filters.update(filter)
		template = environment.get_template(name, globals=self.globals)
		
		return template

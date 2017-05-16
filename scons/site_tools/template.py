#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2011, 2013, Georgi Grinshpun
# Copyright (c) 2012, Niklas Hauser
# Copyright (c) 2012, 2014, Sascha Schade
# Copyright (c) 2013, 2016, Kevin Läufer
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import string
import time
import re
from SCons.Script import *
import posixpath
import traceback

generationBlockString = """/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file.
 */
// ----------------------------------------------------------------------------"""

# -----------------------------------------------------------------------------
def template_action(target, source, env):
	if not env.has_key('substitutions'):
		raise SCons.Errors.UserError, "Use 'Template(..., substitutions = ...)'"
	
	source = source[0].abspath
	target = target[0].abspath
	
	output = string.Template(open(source, 'r').read()).safe_substitute(env['substitutions'])
	
	open(target, 'w').write(output)
	return 0

# -----------------------------------------------------------------------------
def jinja2_template_action(target, source, env):
	if not env.has_key('substitutions'):
		raise SCons.Errors.UserError, "Use 'Jinja2Template(..., substitutions = ...)'"
	
	try:
		import jinja2
	except ImportError:
		env.Error("To use this functionality you need to install the jinja2 template engine")
		Exit(1)

	globals = {
		'time': time.strftime("%d %b %Y, %H:%M:%S", time.localtime()),
		'match': re.match,
		'generation_block': generationBlockString,
	}
	
	def filter_wordwrap(value, width=79):
		return '\n\n'.join([textwrap.fill(str, width) for str in value.split('\n\n')])

	def filter_indent(value, level=0):
		return ('\n' + '\t' * level).join(value.split('\n'))

	def filter_pad(value, min_width):
		tab_width = 4
		tab_count =  (min_width/tab_width - len(value)/tab_width) + 1
		return value + ('\t' * tab_count)

	def filter_split(value, delimiter):
		return value.split(delimiter)

	def filter_values(lst, key):
		"""
		Goes through the list of dictionaries and
		adds all the values of a certain key
		to a list which is thus returned
		"""
		values = []
		for item in lst:
			if isinstance(item, dict) and key in item:
				if item[key] not in values:
					values.append(item[key])
		return values

# Overwrite jinja2 Environment in order to enable relative paths
# since this runs locally that should not be a security concern
# Code from:
# http://stackoverflow.com/questions/8512677/how-to-include-a-template-with-relative-path-in-jinja2
	class RelEnvironment(jinja2.Environment):
		"""Override join_path() to enable relative template paths.
		Take care of paths. Jinja seems to use '/' as path separator in
		templates.
		"""
		def join_path(self, template, parent):
			d = posixpath.join(posixpath.dirname(parent), template)
			return posixpath.normpath(d)

	# path, filename = os.path.split(source[0].path)
	path = env['MODM_ROOTPATH']
	filename = os.path.relpath(source[0].path, path)
	loader = RelEnvironment(loader = jinja2.FileSystemLoader(path), extensions=['jinja2.ext.do'])
	if 'MODM_JINJA2_FILTER' in env:
		loader.filters = dict(loader.filters.items() +
								env['MODM_JINJA2_FILTER'].items())
	loader.filters['modm.wordwrap'] = filter_wordwrap
	loader.filters['modm.indent'] = filter_indent
	loader.filters['modm.pad'] = filter_pad
	loader.filters['modm.values'] = filter_values
	loader.filters['split'] = filter_split	# not MODM specific
	if 'MODM_JINJA2_TEST' in env:
		loader.tests = dict(loader.tests.items() +
								env['MODM_JINJA2_TEST'].items())
	# Jinja2 Line Statements
	loader.line_statement_prefix = '%%'
	loader.line_comment_prefix = '%#'


	try:
		#convert native path format of filename into '/' separated jinja relative path
		template = loader.get_template(filename.replace('\\', '/'), globals=globals)
	except Exception as e:
		#jinja may generate an TemplateNotFound error, which is a subclass of EnvironmentError, but the variable e.errno will be None
		#in SCons/Errors.py in method convert_to_BuildError exceptions which are instance of EnvironmentError are
		#special handled, but need the variable errno be not zero. My guess is, that system generated Environment Errors have errno set to any non zero value.
		#This leads in SCons/Action.py in method __call__ the variable stat.status, s and afterwards the returned result be none and Exception lost.
		#Solution rase Exception, which is not a subclass of EnvironmentError
		#because python cannot chain exceptions e is printed
		traceback.print_exc()
		raise Exception('Failed to retrieve Template',e)

	output = template.render(env['substitutions']).encode('utf-8')
	open(target[0].path, 'w').write(output)

def template_emitter(target, source, env):
	Depends(target, SCons.Node.Python.Value(env['substitutions']))
	return target, source

def template_string(target, source, env):
	return "Template: '%s' to '%s'" % (str(source[0]), str(target[0]))

def template_add_test(env, test_name, test_function, alias='template_add_test'):
	if 'MODM_JINJA2_TEST' not in env:
		env['MODM_JINJA2_TEST'] = {}
	env['MODM_JINJA2_TEST'][test_name] = test_function

def template_add_filter(env, filter_name, filter_function, alias='template_add_filter'):
	if 'MODM_JINJA2_FILTER' not in env:
		env['MODM_JINJA2_FILTER'] = {}
	env['MODM_JINJA2_FILTER'][filter_name] = filter_function

# -----------------------------------------------------------------------------
includeExpression = re.compile(r"(\{%|%%)\s+(import|include)\s+'(?P<file>\S+)'")

def find_includes(file):
	""" Find include directives in an .in file """
	files = []
	for line in open(file).readlines():
		match = includeExpression.search(line)
		if match:
			filename = match.group('file')
			if not os.path.isabs(filename):
				filename = os.path.join(os.path.dirname(os.path.abspath(file)), filename)
			files.append(filename)
	return files

def in_include_scanner(node, env, path, arg=None):
	""" Generates the dependencies for the .in files """
	abspath, targetFilename = os.path.split(node.get_abspath())

	stack = [targetFilename]
	dependencies = [targetFilename]

	while stack:
		nextFile = stack.pop()
		files = find_includes(os.path.join(abspath, nextFile))
		for file in files:
			if file not in dependencies:
				stack.append(file)
				# env.Debug(".in include scanner found %s" % file)
		dependencies.extend(files)

	dependencies.remove(targetFilename)
	return dependencies
# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.Append(
		BUILDERS = {
		'Template': env.Builder(
			action = env.Action(template_action, template_string),
			emitter = template_emitter,
			src_suffix = '.in',
			source_scanner =
				SCons.Script.Scanner(
					function = in_include_scanner,
					skeys = ['.in']),
			single_source = True
		),
		
		'Jinja2Template':  env.Builder(
			action = env.Action(jinja2_template_action, template_string),
			emitter = template_emitter,
			src_suffix = '.in',
			source_scanner =
				SCons.Script.Scanner(
					function = in_include_scanner,
					skeys = ['.in']),
			single_source = True
		),
	})
	env.AddMethod(template_add_test, 'AddTemplateJinja2Test')
	env.AddMethod(template_add_filter, 'AddTemplateJinja2Filter')

def exists(env):
	return True

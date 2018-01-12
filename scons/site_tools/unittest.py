#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Niklas Hauser
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2017, Michael Thies
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

from SCons.Script import *

import os
import re
import string
import datetime

# -----------------------------------------------------------------------------
class FunctionScanner:
	# FIXME correct handling of strings and comments
	commentFilter = re.compile(r"/\*.*?\*/", re.DOTALL)

	# find any function 'void test*();'
	functionFilter = re.compile(r"void\s+(test[A-Z]\w*)\s*\([\svoid]*\)\s*;")

	def __init__(self, filename):
		self.text = open(filename).read()

	def getFunctions(self):
		self._stripCommentsAndStrings()
		functions = self.functionFilter.findall(self.text)
		return functions

	def _stripCommentsAndStrings(self):
		self._prepareString()
		self._stripComments()

		lines = self.text.splitlines()
		for i, line in enumerate(lines):
			lines[i] = self._stripCppComments(line)

		self.text = '\n'.join(lines)

	def _prepareString(self):
		self.text = self.text.replace('\r\n', '\n')
		self.text = self.text.replace('\\\n', '')

	def _stripCppComments(self, line):
		index = line.find(r"//")
		if index >= 0:
			return line[:index]
		return line

	def _stripComments(self):
		self.text = self.commentFilter.sub('', self.text)

# -----------------------------------------------------------------------------
def generateClassName(s):
	words = s.split('_')
	name = []
	for word in words:
		index = 0
		for c in word:
			if c.isalpha():
				break
			index += 1

		name.append(word[0:index])
		name.append(word[index].upper())
		name.append(word[index+1:])

	return ''.join(name)

# -----------------------------------------------------------------------------

def unittest_action(target, source, env):
	if not env.has_key('template'):
		raise SCons.Errors.UserError("Use 'UnittestRunner(..., template = ...)'")

	template = env['template']
	header = source

	tests = {}
	for file in header:
		# io_stream_test.hpp -> io_stream_test
		basename = os.path.splitext(file.name)[0]

		# io_stream_test -> IoStreamTest
		class_name = generateClassName(basename)
		scanner = FunctionScanner(file.abspath)

		tests[class_name] = {
			'include_path': file.abspath,
			'functions': scanner.getFunctions(),
			'test_name': basename,
		}

	includes = []
	name_strings = []
	tests_cases = []
	for class_name, attr in tests.items():
		includes.append('#include "%s"' % attr['include_path'])

		instance_name = class_name[0].lower() + class_name[1:]
		test_name_string = instance_name + 'Name'
		name_strings.append('FLASH_STORAGE_STRING(%s) = "%s";' % (test_name_string, attr['test_name']))

		str = """\
	unittest::Controller::instance().nextTestSuite(modm::accessor::asFlash(%s));
	{
		%s %s;
		""" % (test_name_string, class_name, instance_name)

		for function_name in attr['functions']:
			str += """
		%(instance)s.setUp();
		%(instance)s.%(function)s();
		%(instance)s.tearDown();
	""" % { 'instance': instance_name, 'function': function_name }

		str += "}"

		tests_cases.append(str)

	substitutions = {
		'includes': '\n'.join(includes),
		'names': 'namespace\n{\n\t%s\n}' % '\n\t'.join(name_strings),
		'tests': '\n'.join(tests_cases),
	}

	input = open(os.path.abspath(template), 'r').read()
	output = string.Template(input).safe_substitute(substitutions)
	open(target[0].abspath, 'w').write(output)

	return 0

def unittest_emitter(target, source, env):
	try:
		Depends(target, SCons.Node.Python.Value(env['ARCHITECTURE']))
	except KeyError:
		pass
	header = []
	for file in source:
		if file.name.endswith('_test.hpp'):
			header.append(file)
	return target, header

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.Append(BUILDERS = {
		'UnittestRunner': Builder(
				action = SCons.Action.Action(unittest_action, "Generate runner file: $TARGET"),
				suffix = '.cpp',
				emitter = unittest_emitter,
				target_factory = env.fs.File)
	})

def exists(env):
	return True

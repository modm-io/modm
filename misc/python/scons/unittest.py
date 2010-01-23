#!/usr/bin/env python
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Roboterclub Aachen e.V. nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
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

from SCons.Script import *

import re
import os
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
def unittest_action(target, source, env):
	template = source[0]
	header = source[1:]
	
	#print dir(template)
	tests = {}
	for file in header:
		# io_stream_test.hpp -> io_stream_test
		basename = os.path.splitext(file.name)[0]
		
		# io_stream_test -> IoStreamTest
		class_name = basename.title().replace('_', '')
		scanner = FunctionScanner(file.abspath)
		
		tests[class_name] = {
			'include_path': file.abspath,
			'functions': scanner.getFunctions(),
			'test_name': basename,
		}
	
	includes = []
	name_strings = []
	tests_cases = []
	for class_name, attr in tests.iteritems():
		includes.append('#include "%s"' % attr['include_path'])
		
		instance_name = class_name[0].lower() + class_name[1:]
		test_name_string = instance_name + 'Name'
		name_strings.append('FLASH_STRING(%s) = "%s";' % (test_name_string, attr['test_name']))
		
		str = """\
	unittest::Controller::instance().nextTestSuite(xpcc::toFlashPointer(%s));
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
	
	output = string.Template(open(template.abspath, 'r').read()).safe_substitute(substitutions)
	open(target[0].abspath, 'w').write(output)
	
	return 0
	
def unittest_emitter(target, source, env):
	header = [source[0]]
	for file in source[1:]:
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

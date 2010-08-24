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
Help("""
DESCRIPTION

   This SConstruct file is used to generate the doxygen documentation and some
   other files.

COMMANDLINE ARGUMENTS

all
   Build everythin (this is the default).

doc
doxygen
   Generate the doxygen documentation.

template
templates
   Regenerate some C++ driver classes from templates.

update
   Update the SConstruct files for the projects the tests folder.

unittest
   Run the unittests
   
check
   Check that the examples and tests are compiling.
""")

import os

env = Environment(
		toolpath = ['misc/scons'],
		tools = ['template', 'doxygen', 'configfile', 'helper'],
		ENV = os.environ)

def generateSConstruct(top):
	parser = env.ConfigParser()
	for dir in top:
		for path, directories, files in os.walk(dir):
			# exclude the SVN-directories
			if '.svn' in directories:
				directories.remove('.svn')
			
			if 'project.cfg' in files:
				parser.read(os.path.join(path, 'project.cfg'))
				
				rootpath = os.sep.join(['..' for x in range(len(path.split(os.sep)))])
				file = env.Template(target = os.path.join(path, 'SConstruct'),
									source = 'misc/templates/SConstruct.in',
									substitutions = {'rootpath': rootpath })
				
				env.Alias('update', file)

# regenerate SConstruct files for the tests and examples
generateSConstruct(['tests', 'examples'])

# update all template files
class Generator:
	def __init__(self, env, basepath):
		self.env = env
		self.basepath = basepath
	def template(self, target, source, substitutions):
		self.env.Alias('template',
			self.env.Jinja2Template(
				target = os.path.join(self.basepath, target),
				source = os.path.join(self.basepath, source),
				substitutions = substitutions))

generator = Generator(env, 'src/xpcc/architecture/driver/atmega/uart')
for id in range(0, 4):
	generator.template('uart%i.hpp' % id, 'uart.hpp.in', { 'id': id })
	generator.template('uart%i.cpp' % id, 'uart.cpp.in', { 'id': id })

generator = Generator(env, 'src/xpcc/architecture/driver/atxmega/uart')
for port in ['C', 'D', 'E', 'F']:
	for number in [0, 1]:
		id = "%s%i" % (port, number)
		substitutions = {
			'id': id,
			'number': int(number),
		}
		id = id.lower()
		
		generator.template('uart_%s.hpp' % id, 'uart.hpp.in', substitutions)
		generator.template('uart_%s.cpp' % id, 'uart.cpp.in', substitutions)
		generator.template('uart_buffered_%s.cpp' % id, 'uart_buffered.cpp.in', substitutions)
		generator.template('uart_spi_%s.cpp' % id, 'uart_spi.cpp.in', substitutions)

if 'check' in BUILD_TARGETS:
	result = []
	everythingOk = True
	for path, directories, files in os.walk('examples'):
		# exclude the SVN-directories
		if '.svn' in directories:
			directories.remove('.svn')
	
		if 'SConstruct' in files:
			exitStatus = os.system("scons -Q -C %s" % path)
			result.append("check: %s -> %s" % (path, "Ok" if exitStatus == 0 else "FAIL!"))
			
			if exitStatus != 0:
				everythingOk = False
	
	print '\n'.join(result)
	print "\nOK!" if everythingOk else "\nFAIL!"

env.Alias('check', None)

env.Alias('templates', 'template')

env.Doxygen('doc/doxyfile')
env.Alias('doxygen', 'apidoc/html')
env.Alias('doc', 'doxygen')

env.Phony(show='@firefox doc/apidoc/html/index.html &')
env.Phony(unittest='@scons -Q -C src/')

env.Alias('all', ['doc', 'update', 'templates', 'unittest'])
env.Default('all')

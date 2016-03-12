#!/usr/bin/env python2
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
Help("""
DESCRIPTION

   This SConstruct file is used to generate the doxygen documentation and some
   other files.

COMMANDLINE ARGUMENTS

all
   Build everything (this is the default).

doc
doxygen
   Generate the doxygen documentation.

template
templates
   Regenerate some C++ driver classes from templates.

update
   Update the SConstruct files for the example- and tests-folder.

unittest
   Run the unittests
   
check
   Check that the examples and tests are compiling.
""")

import os

env = Environment(
		toolpath = ['scons/site_tools'],
		tools = ['template', 'doxygen', 'configfile', 'helper', 'font', 'bitmap'],
		ENV = os.environ)
env['XPCC_LIBRARY_PATH'] = os.path.join('.')

class Generator:
	def __init__(self, env, basepath):
		self.env = env
		self.basepath = basepath
	def template(self, target, source, substitutions):
		dd = self.env.Jinja2Template(
						target = os.path.join(self.basepath, target),
						source = os.path.join(self.basepath, source),
						substitutions = substitutions)
		self.env.Alias('template', dd)
		return dd

env['TemplateGenerator'] = Generator

# Generate C++ arrays for gamma corrected led brightness lookup tables
env.SConscript('src/xpcc/ui/led/tables/SConscript.generate', exports='env')

# Generate C++ arrays from the font definition files
env.SConscript('src/xpcc/ui/display/font/SConscript.generate', exports='env')

# Generate C++ arrays from the image files
env.SConscript('src/xpcc/ui/display/image/SConscript.generate', exports='env')

# Generate SConstruct files for all projects in the example/ and tests/ folders
env.SConscript('SConscript.generate', exports='env')

# Provides a 'check' target to compile all examples and the release tests.
env.SConscript('SConscript.check', exports='env')

# Generate the doxygen documentation
env.Doxygen('doc/doxyfile')
env.Alias('doxygen', 'build/api')
env.Alias('doc', 'doxygen')

env.Alias('templates', 'template')

env.Phony(show='@firefox doc/build/api/index.html &')
unittest_str = '@scons -Q -C src/'
# hand values to next scons instance
for key, value in ARGUMENTS.items():
	unittest_str += (" %s=%s" % (key, value))
env.Phony(unittest=unittest_str)

env.Alias('all', ['doc', 'unittest'])
env.Default('all')

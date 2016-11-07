#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2013, 2016, Fabian Greif
# Copyright (c) 2010-2011, Georgi Grinshpun
# Copyright (c) 2011-2013, Niklas Hauser
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, David Hebbeker
# Copyright (c) 2013, Kevin Laeufer
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

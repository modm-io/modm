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

import sys
sys.path.append('misc/python')

from scons.build_properties import PropertyParser, PropertyException

# -----------------------------------------------------------------------------
# create a new commandline-option to specify a property-file

AddOption('--properties',
			dest='properties',
			default='properties.yaml',
			metavar='FILE',
			help='configuration file')

# -----------------------------------------------------------------------------
# parse the global property-file

verbose = ARGUMENTS.get('verbose') == '1'
target = ARGUMENTS.get('target', None)

parser = PropertyParser(GetOption('properties'), target, verbose)

# -----------------------------------------------------------------------------
# create a build-environment for the specific target

build = parser.getGlobalProperties()
if build.target == 'atmega' or build.target == 'atxmega':
	env = Environment(
			ARCHITECTURE = 'avr',
			AVRDUDE = build.avr.avrdude,
			AVR_DEVICE = build.avr.device,
			AVR_CLOCK = build.avr.clock,
			tools = ['avr', 'doxygen', 'template'],
			toolpath = ['misc/python/scons/'],
			LIBS=[''],
			LIBPATH=[])

elif build.target == 'pc':
	env = Environment(
			ARCHITECTURE = 'pc',
			tools = ['pc', 'doxygen', 'template'],
			toolpath = ['misc/python/scons/'],
			LIBS=['boost_thread-mt'],
			LIBPATH=['/usr/lib/'])

else:
	print "Unknown build target '%s'!" % build.target
	Exit(1)

# -----------------------------------------------------------------------------
# finally build the library from the src-directory

sources, header = parser.parseDirectory('src/', 'library')
library = SConscript('src/SConscript',
			src='src',
			variant_dir='build/lib',
			exports=['env', 'sources'], 
			duplicate=False)
lib = env.Alias('lib', 'build/lib')

# -----------------------------------------------------------------------------
# build the tests

#sources, header = parser.parseDirectory('tests/', 'tests')
#SConscript('tests/SConscript',
#			src='tests',
#			variant_dir='build/tests',
#			exports=['env', 'sources', 'library'], 
#			duplicate=False)
#tests = env.Alias('tests', 'build/tests')

# -----------------------------------------------------------------------------
# build the unit tests

sources, header = parser.parseDirectory('unittest/', 'unittest')
SConscript('unittest/SConscript',
			src='unittest',
			variant_dir='build/unittest',
			exports=['env', 'sources', 'header', 'library'])
unittest = env.Alias('unittest', 'build/unittest')

# -----------------------------------------------------------------------------
# create doxygen documentation

env.Doxygen('doc/doxyfile')
env.Alias('doc', 'apidoc/html')

#env.Alias('all', [lib, tests, unittest])
env.Default('lib')

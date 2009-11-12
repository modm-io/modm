#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

def PhonyTargets(env = None, **kw):
	if not env:
		env = DefaultEnvironment()
	for target,action in kw.items():
		env.AlwaysBuild(env.Alias(target, [], action))

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
			tools = ['avr', 'avrdude', 'doxygen', 'template'],
			toolpath = ['misc/python/scons/'],
			LIBS=[''],
			LIBPATH=[])
	
	# set values for avrdude (only if available)
	env['AVRDUDE_PROGRAMMER'] = build.avr.avrdude.get('programmer', 'stk500')
	env['AVRDUDE_PORT'] = build.avr.avrdude.get('port', '/dev/ttyUSB0')
	
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

projects = parser.parseDirectory('src/', 'library')
sources = projects[0].sources

library = SConscript('src/SConscript',
			src='src',
			variant_dir='build/lib',
			exports=['env', 'sources'], 
			duplicate=False)
lib = env.Alias('lib', 'build/lib')

# -----------------------------------------------------------------------------
# build the tests

projects = parser.parseDirectory('tests/', 'tests')
projects = projects[1:]

SConscript('tests/SConscript',
			src='tests',
			variant_dir='build/tests',
			exports=['env', 'projects', 'library'], 
			duplicate=False)
tests = env.Alias('tests', 'build/tests')

# -----------------------------------------------------------------------------
# build the unit tests

projects = parser.parseDirectory('unittest/', 'unittest')
sources = projects[0].sources
header = projects[0].header

SConscript('unittest/SConscript',
			src='unittest',
			variant_dir='build/unittest',
			exports=['env', 'sources', 'header', 'library'])
unittest = env.Alias('unittest', 'build/unittest')

# -----------------------------------------------------------------------------
# create doxygen documentation

env.Doxygen('doc/doxyfile')
env.Alias('doc', 'apidoc/html')

# -----------------------------------------------------------------------------
# other stuff

programFile = ARGUMENTS.get('program', None)
if programFile:
	if env['ARCHITECTURE'] != 'avr':
		print "ERROR: The 'program' target is no available for any non AVR architectures!"
		Exit(0)
	
	# make sure that that the file we want to program is build
	env.Alias("program_target", programFile)
	
	# make sure that 'avrdude' is executed
	PhonyTargets(env, program_target = env.ProgramFlash(programFile))
	
	# build our dummy target
	BUILD_TARGETS.append("program_target")

# show the size of a file
fileSize = ARGUMENTS.get('size', None)
if fileSize:
	# make sure that that the file we want to program is build
	env.Alias("size_target", fileSize)
	
	# make sure that 'avrdude' is executed
	PhonyTargets(env, size_target = env.Size(fileSize))
	
	# build our dummy target
	BUILD_TARGETS.append("size_target")

# show the symbols in a object-file
symbolsFile = ARGUMENTS.get('symbols', None)
if symbolsFile:
	# make sure that that the file we want to program is build
	env.Alias("symbols_target", symbolsFile)
	
	# make sure that 'avrdude' is executed
	PhonyTargets(env, symbols_target = env.Symbols(symbolsFile))
	
	# build our dummy target
	BUILD_TARGETS.append("symbols_target")

# set fusebits
#PhonyTargets(env, fuse = env.ProgramFuses())

# -----------------------------------------------------------------------------

env.Alias('all', [lib, tests, unittest])
env.Default('lib')

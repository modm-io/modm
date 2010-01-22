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
# -----------------------------------------------------------------------------

import sys
sys.path.append('misc/python')

from scons.parser import Parser, ParserException

TOOLPATH = 'misc/python/scons/'

# -----------------------------------------------------------------------------

def PhonyTargets(env = None, **kw):
	if not env:
		env = DefaultEnvironment()
	for target, action in kw.items():
		env.AlwaysBuild(env.Alias(target, [], action))

# -----------------------------------------------------------------------------
# STEP 1: collect information
# 
# Read values form the commandline into variables
# -----------------------------------------------------------------------------

options = {}
options['propertyFile'] = ARGUMENTS.get("properties", "local_properties.yaml")
options['project'] = ARGUMENTS.get("project", "*")

try:
	options['verbose'] = int(ARGUMENTS.get("verbose", "0"))
except ValueError:
	print "Error: Please specifiy a numerical value for 'verbose', '%s' is not allowed" % ARGUMENTS.get("verbose")
	Exit(1)

options['program'] = "avrdude"
if ARGUMENTS.get("program"):
	options['program'] = ARGUMENTS.get("program")
	BUILD_TARGETS.append("program")

# Some properties can be set via the commandline, collect them now
options['properties'] = {
	"target": {},
}

if ARGUMENTS.get("architecture"):
	options['properties']["target"]["architecture"] = ARGUMENTS.get("architecture")

if ARGUMENTS.get("device"):
	options['properties']["target"]["device"] = ARGUMENTS.get("device")

if ARGUMENTS.get("clock"):
	options['properties']["target"]["clock"] = ARGUMENTS.get("clock")

# -----------------------------------------------------------------------------
# STEP 2: Add the documentation build command
# -----------------------------------------------------------------------------

env = Environment(tools = ['doxygen'], toolpath = [ TOOLPATH ])
env.Doxygen('doc/doxyfile')
env.Alias('doc', 'apidoc/html')

env.Default('build')

# -----------------------------------------------------------------------------
# STEP 3: Find the matching projects
# -----------------------------------------------------------------------------

projectList = []	# List of projects which should be build later
try:
	parser = Parser(configuration = options["properties"],
					configFile = options["propertyFile"],
					verbose = options["verbose"])
	
	projects = parser.parseDirectory("unittest")[1:]
	projects += parser.parseDirectory("tests")[1:]
	
	for project in projects:
		# TODO build a selection based on regular expressions
		if project.name == options["project"] or options["project"] == "*":
			projectList.append(project)
	
except ParserException, e:
	print e
	Exit(1)

# -----------------------------------------------------------------------------
# STEP 3: Add all the files which could be build to SCons
# 
# Nothing is build right now, we are just creating a list of possible build
# targets. What really to build is decided later. Using this way single source
# files can be accessed individually by typing 'scons foo.o'. This will only
# build the file 'foo.o'.
# -----------------------------------------------------------------------------

for project in projectList:
	# create a build-environment
	if project.get('architecture') == 'atmega' or \
	   project.get('architecture') == 'atxmega':
		env = Environment(
				ARCHITECTURE = 'avr',
				AVR_DEVICE = project.get('device'),
				AVR_CLOCK = project.get('clock'),
				tools = ['avr', 'avrdude', 'template'],
				toolpath = ['misc/python/scons/'],
				LIBS=[''],
				LIBPATH=[],
				CPPPATH='library/')
		#env['AVRDUDE_PROGRAMMER'] = build.avr.avrdude.get('programmer', 'stk500')
		#env['AVRDUDE_PORT'] = build.avr.avrdude.get('port', '/dev/ttyUSB0')
	elif project.get('architecture') == 'pc':
		env = Environment(
				ARCHITECTURE = 'pc',
				tools = ['pc', 'template'],
				toolpath = ['misc/python/scons/'],
				LIBS=['boost_thread-mt'],
				LIBPATH=['/usr/lib/'],
				CPPPATH='library/')
	else:
		print "Error: Unknown architecture '%s'" % project.get('architecture')
		Exit(1)
	
	# add the library for this project
	# TODO
	#parser = Parser(configuration = options["properties"],
	#				configFile = options["propertyFile"],
	#				verbose = options["verbose"])
	
"""	library = parser.parseDirectory("library")[0]
	
	# build a object file form each source file
	objects = []
	for file in library.sources:
		file.update(env)
		object = env.Object(file.name,
							CPPDEFINES=file.defines)
		objects.append(object)
	
	# link them all together to create the library
	libraryTarget = env.Library(target='robot', source=objects)
	
	# add the project itself
	env.Append(LIBS = 'robot')
	env.Append(LIBPATH = 'build/library/%s' % project.name)
	
	objects = []
	for file in project.sources:
		object = env.Object(target = TODO
							source = file.name,
							CPPDEFINES = file.defines)
		objects.append(object)
	
	programTarget = env.Program(target = project.name,
								source = objects,
								LIBS = project.libs,
								LIBPATH = project.libpath,
								CPPPATH = project.includes)
	
	if env['ARCHITECTURE'] == 'avr':
		env.Hex(programTarget)
		env.Listing(programTarget)
		env.Size(programTarget)
"""
# -----------------------------------------------------------------------------
# STEP 4: Find out what to do and select the things to build
# -----------------------------------------------------------------------------

"""

# transmit the hex file to an AVR
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

# set fusebits (currently only atmega, not the atxmega!)
if build.target == 'atmega':
	PhonyTargets(env, fuse = env.ProgramFuses())

env.Alias('all', [lib, tests, unittest])
"""

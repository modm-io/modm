#!/usr/bin/env python
#
# $Id$

from misc.scons.build_properties import PropertyParser

# create a new commandline-option to specify a property-file
AddOption('--properties',
			dest='properties',
			default='properties.yaml',
			metavar='FILE',
			help='configuration file')

# parse the property-file
parser = PropertyParser(GetOption('properties'), True)

# create a build-environment for the specific target
build = parser.getGlobalProperties()
if build.target == 'avr':
	env = Environment(
			ARCHITECTURE = 'avr',
			AVRDUDE = build.avr.avrdude,
			AVR_DEVICE = build.avr.device,
			AVR_CLOCK = build.avr.clock,
			tools = ['avr'],
			toolpath = ['misc/scons/'])

elif build.target == 'pc':
	env = Environment(
			ARCHITECTURE = 'pc',
			tools = ['pc'],
			toolpath = ['misc/scons/'])

else:
	print "Unknown build target '%s'!" % build.target
	Exit(1)

# finally build the library from the src-directory
sourceFiles = parser.parseDirectory('src/')
SConscript('src/SConscript',
			src='src',
			variant_dir='build', 
			exports=['env', 'sourceFiles'], 
			duplicate=False)

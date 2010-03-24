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

import os
import xpcc_configparser

from SCons.Script import *

# -----------------------------------------------------------------------------
def generate_environment(env, rootpath, configfile='project.cfg', buildpath=None):
	""" Creates a new build environment
	
	Keyword arguments:
	configfile	-	Path to the configuration file
	rootpath	-	Path to the xpcc/trunk directory
	"""
	try:
		parser = xpcc_configparser.XpccConfigParser()
		parser.read(configfile)
		
		# read configuration
		architecture = parser.get('build', 'architecture')
		if architecture == 'pc':
			device = 'pc'
			clock = ''
		else:
			device = parser.get('build', 'device')
			clock = parser.get('build', 'clock')
		
		project_name = parser.get('general', 'name')
		library_name = parser.get('build', 'library_name', project_name)
		
		if buildpath is None:
			buildpath = parser.get('build', 'buildpath', os.path.join(rootpath, 'build/$name'))
		
		rootpath = os.path.abspath(rootpath)
		buildpath = os.path.abspath(buildpath)
	except xpcc_configparser.ParserException, msg:
		print msg
		Exit(1)
	
	configuration = { 'defines': {}, 'environment': {} }
	for section in parser.sections():
		s = {}
		for option in parser.options(section):
			s[option] = parser.get(section, option)
		configuration[section] = s
	
	path_substitutions = {
		'arch': architecture,
		'device': device,
		'name': project_name,
		'library_name': library_name,
	}
	buildpath = string.Template(buildpath).safe_substitute(path_substitutions)
	
	options = {
		'XPCC_LIBRARY_NAME': library_name,
		'XPCC_ROOTPATH': rootpath,
		'XPCC_BUILDPATH': buildpath,
		'XPCC_CONFIG': configuration,
		'XPCC_CONFIG_FILE': os.path.abspath(configfile),
		'XPCC_SYSTEM_BUILDER': os.path.join(rootpath, 'system_design', 'builder')
	}
	
	# tools which are used independent of the architecture
	tools = [
		'doxygen',
		'template',
		'unittest',
		'xpcc',
		'xpcc_configparser',
		'utils',
		'system_design'
	]
	
	# Create the build environment
	if architecture == 'atmega' or architecture == 'atxmega':
		new = Environment(
				ARCHITECTURE = architecture + '/' + device,
				AVR_DEVICE = device,
				AVR_CLOCK = clock,
				tools = ['avr', 'avrdude'] + tools,
				toolpath = env['toolpath'],
				LIBS = [''],
				LIBPATH = [],
				**options)
		
		new['AVRDUDE_PROGRAMMER'] = parser.get('avrdude', 'programmer', 'stk500')
		new['AVRDUDE_PORT'] = parser.get('avrdude', 'port', '/dev/ttyUSB0')
		new['AVR_FUSEBITS'] = []
	elif architecture == 'pc':
		new = Environment(
				ARCHITECTURE = architecture + '/' + device,
				tools = ['pc'] + tools,
				toolpath = env['toolpath'],
				LIBS = ['boost_thread-mt'],
				LIBPATH = ['/usr/lib/'],
				**options)
	else:
		print "Unknown architecture '%s'!" % architecture
		Exit(1)
	
	if 'fusebits' in configuration:
		if architecture == 'atmega':
			fuses = ['lfuse', 'hfuse', 'efuse']
			for key, value in configuration['fusebits'].items():
				if key not in fuses:
					print "Unknown fusebit '%s'! Allowed values are '%s'!" % (key, "', '".join(fuses))
					Exit(1)
				new.Append(AVR_FUSEBITS = {key: value} )
		elif architecture == 'atxmega':
			fuses = ['fuse0', 'fuse1', 'fuse2', 'fuse4', 'fuse5']
			for key, value in configuration['fusebits'].items():
				if key not in fuses:
					print "Unknown fusebit '%s'! Allowed values are '%s'!" % (key, "', '".join(fuses))
					Exit(1)
				new.Append(AVR_FUSEBITS = {key: value} )
		else:
			print "Ignoring 'fusebit' section in project configuration."
	
	# append all values from environment section to the real environment
	for key, value in configuration['environment'].iteritems():
		new.Append(**{ key.upper(): " " + value } )
	
	return new

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(generate_environment, 'GenerateEnvironment')

def exists(env):
	return True

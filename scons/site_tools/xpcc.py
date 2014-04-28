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

import os
import re
import string
import platform
import configfile as configparser

from SCons.Script import *
import SCons.Tool		# to get the SCons default tool search path

# -----------------------------------------------------------------------------
def exclude_from_scanner(path, filename='build.cfg'):
	filename = os.path.join(path, filename)
	dir = os.path.dirname(filename)
	
	if not os.path.exists(dir):
		os.makedirs(dir)
	
	file = open(filename, 'w')
	file.write("""[build]\ntarget = None""")
	file.close()

# -----------------------------------------------------------------------------
def relocate_to_buildpath(env, path, strip_extension=False):
	""" Relocate path from source directory to build directory
	"""
	path = str(path)
	if strip_extension:
		path = os.path.splitext(path)[0]
	
	path = os.path.relpath(path, env['XPCC_BASEPATH'])
	if path.startswith('..'):
		# if the file is not in a subpath of the current directory
		# build it in the root directory of the build path
		while path.startswith('..'):
			path = path[3:]
		#path = os.path.basename(path)
	
	return os.path.abspath(os.path.join(env['XPCC_BUILDPATH'], path))

def check_architecture(env, architecture):
	if re.match(architecture, env['ARCHITECTURE']):
		return True
	else:
		return False

def show_defines_action(target, source, env):
	projectConfig = env['XPCC_CONFIG']['defines'].keys()
	keys = env['XPCC_LIBRARY_DEFINES'].keys()
	keys.sort()
	
	print ""
	print "XPCC:"
	for key in keys:
		default = env['XPCC_LIBRARY_DEFAULT_DEFINES'][key]
		if key in projectConfig:
			value = env['XPCC_CONFIG']['defines'][key]
			projectConfig.remove(key)
			if value != default:
				print "  %s => %s (default: %s)" % (key.upper(), value, default)
				continue
		
		print "  %s => %s" % (key.upper(), default)
	
	if projectConfig:
		print "\nOther:"
		for key in projectConfig:
			print "  %s => %s" % (key.upper(), env['XPCC_CONFIG']['defines'][key])
	print ""

def show_defines(env, alias="__show"):
	action = Action(show_defines_action, cmdstr="Show Defines:")
	return env.AlwaysBuild(env.Alias(alias, [], action))

def xpcc_library(env, buildpath=None):
	env.AppendUnique(CPPPATH = env['XPCC_LIBRARY_PATH'])
	env.AppendUnique(CPPPATH = os.path.join(env['XPCC_ROOTPATH'], 'ext'))
	
	backup = env['XPCC_BUILDPATH'], env['XPCC_BASEPATH']
	
	# set a new buildpath for the library
	if buildpath is None:
		buildpath = os.path.join(env['XPCC_BUILDPATH'], 'libxpcc')
	env['XPCC_BUILDPATH'] = os.path.abspath(buildpath)
	env['XPCC_BASEPATH'] = env['XPCC_ROOTPATH']
	
	# exclude the buildpath from the FileScanner
	exclude_from_scanner(env['XPCC_BUILDPATH'])

	# exclude the generated path from the FileScanner
	exclude_from_scanner(env['XPCC_PLATFORM_GENERATED_PATH'])
	
	# build the library
	library, defines = env.SConscript(
			os.path.join(env['XPCC_ROOTPATH'], 'src', 'SConscript'),
			exports = 'env')
	
	# generate 'xpcc_config.hpp'
	env['XPCC_LIBRARY_DEFINES'] = defines.copy()
	env['XPCC_LIBRARY_DEFAULT_DEFINES'] = defines.copy()
	for key in defines.iterkeys():
		if key in env['XPCC_CONFIG']['defines']:
			env['XPCC_LIBRARY_DEFINES'][key] = env['XPCC_CONFIG']['defines'][key]
	
	define_list = ["#define %s %s" % (key.upper(), value) \
				for key, value in env['XPCC_LIBRARY_DEFINES'].iteritems()]
	define_list.sort()
	
	substitutions = {
		'defines': '\n'.join(define_list),
		'name': env['XPCC_CONFIG']['general']['name']
	}

	file = env.Template(
			target = os.path.join(env['XPCC_BUILDPATH'], 'xpcc_config.hpp'),
			source = os.path.join(env['XPCC_ROOTPATH'], 
								  'templates/xpcc_config.hpp.in'),
			substitutions = substitutions)
	
	env.PrependUnique(LIBS = ['xpcc'])
	env.PrependUnique(LIBPATH = [os.path.join(env['XPCC_BUILDPATH'], 'src')])
	
	# restore original environment
	env['XPCC_BUILDPATH'], env['XPCC_BASEPATH'] = backup
	
	return library

def xpcc_communication_header(env, xmlfile, path='.'):
	files  = env.SystemCppPackets(xmlfile, path=path)
	files += env.SystemCppIdentifier(xmlfile, path=path)
	if 'communication' in env['XPCC_CONFIG']:
		files += env.SystemCppPostman(
				target='postman',
				source=xmlfile,
				container=env['XPCC_CONFIG']['communication']['container'],
				path=path)
	
	source = []
	for file in files:
		if file.name.endswith('.cpp'):
			source.append(file)
	
	return source

def generate_defines(env, filename='defines.hpp'):
	defines = env['XPCC_CONFIG']['defines']
	substitutions = {
		'defines': '\n'.join(["#define %s %s" % (key.upper(), value) \
				for key, value in defines.iteritems()])
	}
	file = env.Template(
			target = filename,
			source = os.path.join(env['XPCC_ROOTPATH'], 
								  'templates/defines.hpp.in'),
			substitutions = substitutions)
	return file

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# We make sure that the installed version is 2.6 or higher because
	# features from this version like os.path.relpath() are used.
	EnsurePythonVersion(2, 6)
	EnsureSConsVersion(1, 0)

	# Import Logger Tool
	env.Tool('logger_tools')
	log_level = ARGUMENTS.get('LOG_LEVEL', None)
	if log_level == None:
		log_level = ARGUMENTS.get('ll', None)
	if log_level == None:
		log_level = 'warn'
	env.SetLogLevel(log_level)

	# Chose Compiler
	c = ARGUMENTS.get('COMPILER', None)
	if c == None:
		c = ARGUMENTS.get('compiler', None)
	if c == None:
		c = ARGUMENTS.get('c', None)
	env['XPCC_COMPILER'] = c

	# detect the rootpath to the xpcc folder
	rootpath = env.get('rootpath')
	
	if rootpath is None:
		# Check if a global environment variable exists that point to
		# the root of the xpcc-folder
		rootpath = ARGUMENTS.get('XPCC_HOME', None)
		if rootpath is not None:
			env.Info("Use path from 'XPCC_HOME': '%s'" % rootpath)
	
	if rootpath is None:
		# try to detect the rootpath
		sitepath = os.path.join('scons', 'site_tools')
		
		# DefaultToolpath is used when the xpcc toolpath is added via --site-dir=$/xpcc/scons
		searchpath = env.get('toolpath', []) + SCons.Tool.DefaultToolpath
		
		for path in searchpath:
			path = os.path.normpath(path)
			if path.endswith(sitepath):
				p = path[:-len(sitepath) - 1]
				# To avoid false detection of directories with a similar
				# name check if at least a 'src' directory exists 
				if os.path.exists(os.path.join(p, 'src')):
					rootpath = p
					env.Info("Extracted path from tool path: '%s'" % rootpath)
					break
		
		if rootpath is None:
			env.Error("Could not detect the path to the xpcc-library. Use " \
				  "'Environment(rootpath=...)' to set the path to the root folder of xpcc.")
			env.Exit(1)
	rootpath = os.path.abspath(rootpath)
	
	# load the configuration file
	configfile = ARGUMENTS.get('config', env.get('configfile', 'project.cfg'))
	try:
		parser = configparser.Parser()
		parser.read(configfile)

		device = parser.get('build', 'device')

		hosted_device = {'Darwin': 'darwin', 'Linux': 'linux',
						 'Windows': 'windows' }[platform.system()]
		device = string.Template(device).safe_substitute({'hosted': hosted_device})

		clock  = parser.get('build', 'clock', 'NaN')

		architecture_derecated = parser.get('build', 'architecture', 'deprecated')
		if architecture_derecated != "deprecated":
			env.Warn("Specifying architecture is deprecated and replaced only by the Device ID.")

		projectName = parser.get('general', 'name')

		buildpath = env.get('buildpath')
		# put together build path
		if buildpath is None:
			buildpath = parser.get('build', 'buildpath', os.path.join(os.curdir, 'build/'))

		# load parameters if available
		env['XPCC_USER_PARAMETERS'] = parser.items('parameters')

	except configparser.ParserException, msg:
		env.Error("Error parsing file configuration file '%s':\n%s" % (configfile, str(msg)))
		Exit(1)

	configuration = { 'defines': {}, 'environment': {} }
	for section in parser.sections():
		s = {}
		for option in parser.options(section):
			s[option] = parser.get(section, option)
		configuration[section] = s


	env['XPCC_ROOTPATH'] = rootpath			# xpcc rootpath
	env['XPCC_BASEPATH'] = os.curdir		# path of the current project
	env['XPCC_LIBRARY_PATH'] = os.path.join(rootpath, 'src')
	env['XPCC_CONFIG'] = configuration
	env['XPCC_CONFIG_FILE'] = os.path.abspath(configfile)
	env['XPCC_SYSTEM_BUILDER'] = os.path.join(rootpath, 'tools', 'system_design', 'builder')
	env['XPCC_DEVICE'] = device 			# needed by the platform tools

	# tools which are used independently of the architecture
	env.Tool('template') # needs to be added before platform_tools
	env.Tool('unittest')
	env.Tool('configfile')
	env.Tool('helper')
	env.Tool('system_design')
	# Will validate the env['XPCC_DEVICE'] and set env['ARCHITECTURE']
	env.Tool('platform_tools')

	env.FindDeviceFile()

	buildpath = string.Template(buildpath).safe_substitute({
				'arch': env['ARCHITECTURE'],
				'device': device,
				'name': projectName,
				'xpccpath': rootpath
			})
	buildpath = os.path.abspath(buildpath)
	# exclude the buildpath from the FileScanner
	exclude_from_scanner(buildpath)
	env['XPCC_BUILDPATH'] = buildpath

	env['LIBS'] = ['']
	env['LIBPATH'] = []
	env['CPPPATH'] = []

	# architecture specific settings and tools
	if env['ARCHITECTURE'] == 'avr8':
		env['AVR_DEVICE'] = device
		env['AVR_CLOCK'] = clock
		env['LIBS'] = ['']
		env['LIBPATH'] = []

		env.Tool('avr')

		if parser.has_section('avrdude'):
			env.Tool('avrdude')
			env['AVRDUDE_PROGRAMMER'] = parser.get('avrdude', 'programmer')
			env['AVRDUDE_PORT'] = parser.get('avrdude', 'port')
			env['AVRDUDE_OPTIONS'] = ""
			env['AVRDUDE_BAUDRATE'] = []
			env['AVRDUDE_OPTIONS'] = []
			for key, value in configuration['avrdude'].items():
				if key == 'baudrate':
					env['AVRDUDE_BAUDRATE'] = value
				if key == 'options':
					env['AVRDUDE_OPTIONS'] = value
			env['AVR_FUSEBITS'] = []

			if 'fusebits' in configuration:
				if device.startswith('atmega') or device.startswith('attiny') or device.startswith('at90can'):
					fuses = ['lfuse', 'hfuse', 'efuse']
					for key, value in configuration['fusebits'].items():
						if key not in fuses:
							env.Error("Unknown fusebit '%s'! Allowed values are '%s'!" % \
									(key, "', '".join(fuses)))
							Exit(1)
						env.Append(AVR_FUSEBITS = {key: value} )
				elif device.startswith('atxmega'):
					fuses = ['fuse0', 'fuse1', 'fuse2', 'fuse4', 'fuse5']
					for key, value in configuration['fusebits'].items():
						if key not in fuses:
							env.Error("Unknown fusebit '%s'! Allowed values are '%s'!" % \
									(key, "', '".join(fuses)))
							Exit(1)
						env.Append(AVR_FUSEBITS = {key: value} )
				else:
					env.Warn("Ignoring 'fusebit' section in project configuration. Unknown device %s" % device)

		# path to the headers of a very small and incomplete libstdc++ implementation
		env.Append(CPPPATH = [os.path.join(rootpath, 'src', 'stdc++')])

	elif env['ARCHITECTURE'] == 'hosted':
		if device == 'linux':
			libs = ['boost_thread-mt', 'boost_system']
			libpath = ['/usr/lib/']
		else:
			libs = []
			libpath = []
		env['CXXCOM'] = []
		env['LINKCOM'] = []
		env['LIBS'] = libs
		env['LIBPATH'] = libpath
		env['ENV'] = os.environ
		
		env.Tool('hosted')
	elif env['ARCHITECTURE'] in ['arm7tdmi', 'cortex-m0', 'cortex-m3', 'cortex-m4', 'cortex-m4f']:
		if env['ARCHITECTURE'] == 'cortex-m4f':
			env['ARM_ARCH'] = 'cortex-m4'
		else:
			env['ARM_ARCH'] = env['ARCHITECTURE']
		env['ARM_DEVICE'] = device
		env['ARM_CLOCK'] = clock
		
		env.Tool('arm')
		
		# load openocd tool if required
		if parser.has_section('program'):
			try:
				if parser.get('program', 'tool') == 'openocd':
					env.Tool('openocd')
					env['OPENOCD_CONFIGFILE'] = parser.get('openocd', 'configfile')
					env['OPENOCD_COMMANDS'] = parser.get('openocd', 'commands')
				if parser.get('program', 'tool') == 'stlink':
					env.Tool('stlink')
				if parser.get('program', 'tool') == 'lpclink':
					env['GDB_PORT'] = parser.get('debug', 'gdbport')
					env.Tool('lpclink')
					#env['OPENOCD_CONFIGFILE'] = parser.get('openocd', 'configfile')
					#env['OPENOCD_COMMANDS'] = parser.get('openocd', 'commands')
			except configparser.ParserException as e:
				env.Error("Error in Configuration: %s" % e)
				Exit(1)
		if parser.has_section('debug'):
			try:
				if parser.get('debug', 'tool') == 'gdb':
					env['GDB_PORT'] = parser.get('debug', 'gdbport')
					env.Tool('gdb')
			except configparser.ParserException as e:
				env.Error("Error in Configuration: %s" % e)
				Exit(1)			
	elif env['ARCHITECTURE'] == 'avr32':
		env['AVR32_DEVICE'] = device
		env['AVR32_CLOCK']  = clock
		env['LIBS']         = ['']
		env['LIBPATH']      = []
		env.Tool('avr32')
		env.Tool('dfu-programmer')
	else:
		env.Error("xpcc Error: Unknown architecture '%s'!" % env['ARCHITECTURE'])
		Exit(1)

	# append all values from environment section to the real environment
	for key, value in configuration['environment'].iteritems():
		if key.endswith('*'):
			key = key[:-1]
			value = " " + value
		if key.upper() == "CPPPATH":
			value = value.split(':')
		env.Append(**{ key.upper(): value } )
	
	# These emitters are used to build everything not in place but in a
	# separate build-directory.
	def defaultEmitter(target, source, env):
		targets = []
		for file in target:
			# relocate the output to the buildpath
			filename = env.Buildpath(file.path)
			targets.append(filename)
		return targets, source
	
	env['BUILDERS']['Object'].add_emitter('.cpp', defaultEmitter)
	env['BUILDERS']['Object'].add_emitter('.c', defaultEmitter)
	env['BUILDERS']['Object'].add_emitter('.sx', defaultEmitter)
	env['BUILDERS']['Object'].add_emitter('.S', defaultEmitter)
	
	env['LIBEMITTER'] = defaultEmitter
	env['PROGEMITTER'] = defaultEmitter
	
	env.AddMethod(relocate_to_buildpath, 'Buildpath')
	env.AddMethod(check_architecture, 'CheckArchitecture')
	env.AddMethod(show_defines, 'ShowDefines')
	env.AddMethod(xpcc_library, 'XpccLibrary')
	env.AddMethod(xpcc_communication_header, 'XpccCommunication')
	env.AddMethod(generate_defines, 'Defines')

def exists(env):
	return True

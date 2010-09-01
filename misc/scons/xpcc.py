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
import re
import string
import platform
import configfile as configparser

from SCons.Script import *

# -----------------------------------------------------------------------------
def excludeFromScanner(path, filename='build.cfg'):
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
	
	if os.path.isabs(path) or path.startswith('..'):
		path = os.path.relpath(path, env['XPCC_BASEPATH'])
		if path.startswith('..'):
			# if the file is not in a subpath of the current directory
			# build it in the root directory of the build path
			path = os.path.basename(path)
	
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
	env.Append(CPPPATH = env['XPCC_LIBRARY_PATH'])
	
	backup = env['XPCC_BUILDPATH'], env['XPCC_BASEPATH']
	
	# set a new buildpath for the library
	if buildpath is None:
		buildpath = os.path.join(env['XPCC_BUILDPATH'], 'libxpcc')
	env['XPCC_BUILDPATH'] = os.path.abspath(buildpath)
	env['XPCC_BASEPATH'] = env['XPCC_LIBRARY_PATH']
	
	# exclude the buildpath from the FileScanner
	excludeFromScanner(env['XPCC_BUILDPATH'])
	
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
	
	substitutions = {
		'defines': '\n'.join(["#define %s %s" % (key.upper(), value) \
				for key, value in env['XPCC_LIBRARY_DEFINES'].iteritems()]),
		'name': env['XPCC_CONFIG']['general']['name']
	}
	file = env.Template(
			target = os.path.join(env['XPCC_BUILDPATH'], 'xpcc_config.hpp'),
			source = os.path.join(env['XPCC_ROOTPATH'], 
								  'misc/templates/xpcc_config.hpp.in'),
			substitutions = substitutions)
	
	env.Append(LIBS = ['xpcc'])
	env.Append(LIBPATH = [env['XPCC_BUILDPATH']])
	
	# restore original environment
	env['XPCC_BUILDPATH'], env['XPCC_BASEPATH'] = backup
	
	return library

def xpcc_communication_header(env, xmlfile):
	# TODO
	env.Append(CPPPATH = os.path.join(os.path.dirname(xmlfile), ".."))
	
	files  = env.SystemCppPackets(xmlfile)
	files += env.SystemCppIdentifier(xmlfile)
	if 'communication' in env['XPCC_CONFIG']:
		files += env.SystemCppPostman(target='postman.hpp',
				source=xmlfile,
				container=env['XPCC_CONFIG']['communication']['container'])
	
	source = []
	for file in files:
		if file.name.endswith('.cpp'):
			source.append(file)
	
	return source

def generate_defines(env, filename='defines.hpp'):
	defines = env['XPCC_CONFIG']['defines']
	if defines:
		substitutions = {
			'defines': '\n'.join(["#define %s %s" % (key.upper(), value) \
					for key, value in defines.iteritems()])
		}
		file = env.Template(
				target = filename,
				source = os.path.join(env['XPCC_ROOTPATH'], 
									  'misc/templates/defines.hpp.in'),
				substitutions = substitutions)
		return file
	else:
		return None

# -----------------------------------------------------------------------------
def generate(env, **kw):
	EnsurePythonVersion(2, 6)
	
	# detect the rootpath to the xpcc folder
	rootpath = env.get('rootpath')
	if rootpath is None:
		# try to detect the rootpath
		sitepath = os.path.join('misc', 'scons')
		for path in env['toolpath']:
			path = os.path.normpath(path)
			if path.endswith(sitepath):
				rootpath = path[:-len(sitepath) - 1]
				break
		
		if rootpath is None:
			print "Could not detect the path to the xpcc-library. Use " \
				  "'Environment(rootpath=...)' to set the path to the root folder of xpcc."
			env.Exit(1)
	rootpath = os.path.abspath(rootpath)
	
	# load the configuration file
	configfile = ARGUMENTS.get('config', env.get('configfile', 'project.cfg'))
	try:
		parser = configparser.Parser()
		parser.read(configfile)
		
		architecture = parser.get('build', 'architecture')
		if architecture == 'pc':
			try:
				defaultDevice = {
					'Darwin': 'darwin',
					'Linux': 'linux',
					'Windows': 'windows' }[platform.system()]
				device = parser.get('build', 'device', defaultDevice)
			except KeyError, msg:
				print "Error: unknown platform: '%s' " % msg
			clock = ''
		else:
			device = parser.get('build', 'device')
			clock = parser.get('build', 'clock')
		
		projectName = parser.get('general', 'name')
		
		buildpath = env.get('buildpath')
		if buildpath is None:
			buildpath = parser.get('build', 'buildpath', os.path.join(os.curdir, 'build/'))
		
		buildpath = string.Template(buildpath).safe_substitute({
					'arch': architecture,
					'device': device,
					'name': projectName,
					'xpccpath': rootpath
				})
		buildpath = os.path.abspath(buildpath)
		
		# exclude the buildpath from the FileScanner
		excludeFromScanner(buildpath)
	except configparser.ParserException, msg:
		print "Error parsing file configuration file '%s':\n%s" % (configfile, str(msg))
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
	env['XPCC_BUILDPATH'] = buildpath
	env['XPCC_CONFIG'] = configuration
	env['XPCC_CONFIG_FILE'] = os.path.abspath(configfile)
	env['XPCC_SYSTEM_BUILDER'] = os.path.join(rootpath, 'system_design', 'builder')
	
	# tools which are used independent of the architecture
	env.Tool('template')
	env.Tool('unittest')
	env.Tool('configfile')
	env.Tool('helper')
	env.Tool('system_design')
	
	# architecture specific settings and tools
	env['ARCHITECTURE'] = architecture + '/' + device
	if architecture == 'avr':
		env['AVR_DEVICE'] = device
		env['AVR_CLOCK'] = clock
		env['LIBS'] = ['']
		env['LIBPATH'] = []
		
		env.Tool('avr')
		
		if parser.has_section('avrdude'):
			env.Tool('avrdude')
			env['AVRDUDE_PROGRAMMER'] = parser.get('avrdude', 'programmer')
			env['AVRDUDE_PORT'] = parser.get('avrdude', 'port')
			env['AVR_FUSEBITS'] = []
			
			if 'fusebits' in configuration:
				if device.startswith('atmega') or device.startswith('attiny'):
					fuses = ['lfuse', 'hfuse', 'efuse']
					for key, value in configuration['fusebits'].items():
						if key not in fuses:
							print "Unknown fusebit '%s'! Allowed values are '%s'!" % \
									(key, "', '".join(fuses))
							Exit(1)
						env.Append(AVR_FUSEBITS = {key: value} )
				elif device.startswith('atxmega'):
					fuses = ['fuse0', 'fuse1', 'fuse2', 'fuse4', 'fuse5']
					for key, value in configuration['fusebits'].items():
						if key not in fuses:
							print "Unknown fusebit '%s'! Allowed values are '%s'!" % \
									(key, "', '".join(fuses))
							Exit(1)
						env.Append(AVR_FUSEBITS = {key: value} )
				else:
					print "Ignoring 'fusebit' section in project configuration."
		
		# path to the headers of a very small and incomplete libstdc++ implementation
		env.Append(CPPPATH = [os.path.join(rootpath, 'src', 'stdc++')])
		
	elif architecture == 'pc':
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
		env['CPPPATH'] = []
		env['ENV'] = os.environ
		
		env.Tool('pc')
	else:
		print "Unknown architecture '%s'!" % architecture
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

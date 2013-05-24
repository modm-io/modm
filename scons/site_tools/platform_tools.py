#!/usr/bin/env python
# 
# Copyright (c) 2013, Roboterclub Aachen e.V.
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
# -----------------------------------------------------------------------------
# 
# DESCRIPTION
# 
# This tool checks which files are needed for a specific target
# using the xml device file and adds a custom builder for all template files
# and for other files that need to be copied to the generated folder
#
# WARNING: Do NOT name this file platform.py because this overrides a
#          different platform module used by /usr/lib/scons/SCons/Tool/tex.py


from SCons.Script import *
import os
from string import Template

from configfile import Scanner # for header and source file endings

# add device_file module from tools to path
# this is apparently not pythonic, but I see no other way to do this
# without polluting the site_tools directory or haveing duplicate code
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools', 'device_files'))
from device import DeviceFile
from driver import DriverFile


#------------------------------------------------------------------------------
#
#
#
# env['XPCC_PLATFORM_PATH'] is used for abolute paths
# architecture_path for relative build paths
def platform_tools_generate(env, architecture_path):
	device = env['XPCC_DEVICE']
	# Initialize Return Lists/Dicts
	sources = []
	defines = {}
	includes = []
	# make paths
	platform_path = os.path.join(architecture_path, 'platform')
	generated_path = os.path.join(architecture_path, env['XPCC_PLATFORM_GENERATED_DIR'])
	# Find Device File
	xml_path = os.path.join(env['XPCC_PLATFORM_PATH'], 'xml')
	files = []
	device_file = None
	while device != None and len(device) > 0:
		device_file = os.path.join(xml_path, device + '.xml')
		files.append(device_file)
		if os.path.isfile(device_file):
			break
		else:
			device = device[:-1]
			device_file = None
	device = env['XPCC_DEVICE'] # restore device

	# Check for error
	if device_file == None:
		emsg = "XPCC Error: Could not find xml device file." + os.linesep
		for f in files:
			emsg += "Tried: " + f + os.linesep
		raise SCons.Errors.BuildError(node=target, errstr=emsg)
	print "Using Xml Device File '%s'" % os.path.basename(device_file)
	# Now we need to parse the Xml File
	dev = DeviceFile(device_file)

	# Parse Properties
	prop = dev.getProperties(device)
	defines = prop['defines']
	# FIXME: This is a hack to make everything build without arm_devices.py
	# We realy need to look into which defines we want to be available via a
	# xpp_config.hpp and which via a command line option
	env.Append(CPPDEFINES = defines)
	device_headers = prop['headers']
	# Set Size
	env['DEVICE_SIZE'] = { "flash": prop['flash'], "ram": prop['ram'] }
	# Find Linkerscript:
	linkerfile = os.path.join(env['XPCC_PLATFORM_PATH'], 'linker', prop['linkerscript'])
	if not os.path.isfile(linkerfile):
		linkerfile = os.path.join(env['XPCC_PLATFORM_PATH'], 'linker', prop['target']['platform'], prop['linkerscript'])
		if not os.path.isfile(linkerfile):
			print "Linkerscript for %s (%s) could not be found." % (device, linkerfile)
			Exit(1)
	linkdir, linkfile = os.path.split(linkerfile)
	linkdir = linkdir.replace(env['XPCC_ROOTPATH'], "${XPCC_ROOTPATH}")
	env['LINKPATH'] = str(linkdir)
	env['LINKFILE'] = str(linkfile)

	# Loop through Drivers
	device_substitutions = {} # Substitutions for the drivers.hpp.in file
	device_substitutions['drivers'] = []
	drivers = dev.getDriverList(device, env['XPCC_PLATFORM_PATH'])
	for driver in drivers:
		ddic = {} # create dictionary describing the driver
		d = DriverFile.fromDict(driver)
		ddic['name'] = d.name
		ddic['type'] = d.type
		ddic['headers'] = []
		build = d.getBuildList(env['XPCC_PLATFORM_PATH'])
		for f in build:
			src = os.path.join(platform_path, f[0])
			tar = os.path.join(generated_path, f[1])
			if len(f) == 3:
				res = env.Jinja2Template(target = tar, source = src, substitutions = f[2])
			else:
				res = env.Command(tar, src, Copy("$TARGET", "$SOURCE"))
			# check if target is header file
			if os.path.splitext(tar)[1] in Scanner.HEADER:
				ddic['headers'].append(f[1]) # append path relative to platform dir
				includes.append(os.path.dirname(tar))
			# or source file
			elif os.path.splitext(tar)[1] in Scanner.SOURCE:
				sources.append(res)
		device_substitutions['drivers'].append(ddic)

	####### Generate Header Templates #########################################
	# Show SCons how to build the architecture/platform.hpp file:
	src = os.path.join(platform_path, 'platform.hpp.in')
	tar = os.path.join(architecture_path, 'platform.hpp')
	sub = {'device': device}
	env.Template(target = tar, source = src, substitutions = sub)
	# Show SCons how to build the drivers.hpp.in file:
	src = os.path.join(platform_path, 'drivers.hpp.in')
	tar = os.path.join(generated_path, 'drivers.hpp')
	env.Jinja2Template(target = tar, source = src, substitutions = device_substitutions)
	# Show SCons how to build external.hpp.in file:
	src = os.path.join(platform_path, 'external.hpp.in')
	tar = os.path.join(generated_path, 'external.hpp')
	sub = {'headers': device_headers}
	env.Jinja2Template(target = tar, source = src, substitutions = sub)
	return sources, defines, includes

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# Set some paths used by this file
	env['XPCC_PLATFORM_PATH'] = \
		os.path.join(env['XPCC_LIBRARY_PATH'], 'xpcc', 'architecture', 'platform')
	env['XPCC_PLATFORM_GENERATED_DIR'] = 'generated_platform_' + env['XPCC_DEVICE']
	env['XPCC_PLATFORM_GENERATED_PATH'] = \
		os.path.join(env['XPCC_LIBRARY_PATH'], 'xpcc', 'architecture', env['XPCC_PLATFORM_GENERATED_DIR'])

	# Add Method to Parse XML Files, and create Template / Copy Dependencies
	env.AddMethod(platform_tools_generate, 'GeneratePlatform')

# -----------------------------------------------------------------------------
def exists(env):
	return True

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
# without polluting the site_tools directory or having duplicate code
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools', 'device_files'))
from device import DeviceFile
from device_identifier import DeviceIdentifier
from driver import DriverFile
from parameters import ParameterDB


#------------------------------------------------------------------------------
#
def platform_tools_find_device_file(env):
	architecture_path = os.path.join(env['XPCC_LIBRARY_PATH'], 'xpcc', 'architecture')
	device = env['XPCC_DEVICE']
	# Do not generate for hosted
	# TODO: generate software peripherals for hosted
	if device in ['darwin', 'linux', 'windows']:
		env['ARCHITECTURE'] = 'hosted/'+device
		return
	id = DeviceIdentifier(device)
	env.Debug("Device String: %s" % device)
	# Find Device File
	xml_path = os.path.join(env['XPCC_PLATFORM_PATH'], 'xml', id.platform)
	files = []
	device_file = None
	if id.platform == 'avr':
		for file in os.listdir(xml_path):
			if id.family in file:
				fileArray = file.replace(id.family,"").replace(".xml","").split("-")
				names = fileArray[0].split("_")
				types = fileArray[1].split("_")
				pinIdString = fileArray[2] if len(fileArray) > 2 else None
				if id.name in names:
					type  = id.type
					if type == None:
						type = 'none'
					if type in types:
						if id.family == 'xmega' and id.pin_id != pinIdString:
							continue
						device_file = os.path.join(xml_path, file)
						break
	elif id.platform == 'stm32':
		for file in os.listdir(xml_path):
			if 'stm32'+id.family in file:
				fileArray = file.replace('stm32'+id.family[0],'').replace('.xml','').split('-')
				if len(fileArray) < 3:
					continue
				names = fileArray[0].split('_')
				pins = fileArray[1].split('_')
				sizes = fileArray[2].split('_')
				if id.name in names and id.pin_id in pins and id.size_id in sizes:
					device_file = os.path.join(xml_path, file)
					break
	else:
		while device != None and len(device) > 0:
			device_file = os.path.join(xml_path, device + '.xml')
			files.append(device_file)
			if os.path.isfile(device_file):
				break
			else:
				device = device[:-1]
				device_file = None

	# Restore Device
	device = env['XPCC_DEVICE']

	# Check for error
	if device_file == None:
		env.Error("XPCC Error: Could not find xml device file." + os.linesep)
		# for f in files:
		#	env.Error("Tried: " + f + os.linesep)
		Exit(1)
	# Now we need to parse the Xml File
	env.Debug("Found device file: " + device_file)
	env['XPCC_DEVICE_FILE'] = DeviceFile(device_file, env.GetLogger())
	# for microcontrollers architecture = core
	env['ARCHITECTURE'] = env['XPCC_DEVICE_FILE'].getProperties(device)['core']
	if id.platform == 'avr':
		env['AVRDUDE_DEVICE'] = env['XPCC_DEVICE_FILE'].getProperties(device)['mcu']

#------------------------------------------------------------------------------
# env['XPCC_PLATFORM_PATH'] is used for absolute paths
# architecture_path for relative build paths
def platform_tools_generate(env, architecture_path):
	device = env['XPCC_DEVICE']
	# Do not generate for hosted
	# TODO: generate software peripherals for hosted
	if device in ['darwin', 'linux', 'windows']:
		return [], {}, []

	# Initialize Return Lists/Dicts
	sources = []
	defines = {}
	includes = []
	# make paths
	platform_path = os.path.join(architecture_path, 'platform')
	generated_path = os.path.join(architecture_path, env['XPCC_PLATFORM_GENERATED_DIR'])

	dev = env['XPCC_DEVICE_FILE']

	# Parse Properties
	prop = dev.getProperties(device)
	env.Debug("Found properties: %s" % prop)
	defines = prop['defines']
	# FIXME: This is a hack to make everything build without arm_devices.py
	# We really need to look into which defines we want to be available via a
	# xpcc_config.hpp and which via a command line option
	env.Append(CPPDEFINES = defines)
	device_headers = prop['headers']

	# Set Size
	env['DEVICE_SIZE'] = { "flash": prop['flash'], "ram": prop['ram'], "eeprom": prop['eeprom'] }
	if (prop['linkerscript'] != ""):
		# Find Linkerscript:
		linkerfile = os.path.join(env['XPCC_PLATFORM_PATH'], 'linker', prop['linkerscript'])
		if not os.path.isfile(linkerfile):
			linkerfile = os.path.join(env['XPCC_PLATFORM_PATH'], 'linker', prop['target']['platform'], prop['linkerscript'])
			if not os.path.isfile(linkerfile):
				env.Error("Linkerscript for %s (%s) could not be found." % (device, linkerfile))
				Exit(1)
		linkdir, linkfile = os.path.split(linkerfile)
		linkdir = linkdir.replace(env['XPCC_ROOTPATH'], "${XPCC_ROOTPATH}")
		env['LINKPATH'] = str(linkdir)
		env['LINKFILE'] = str(linkfile)
	else:
		env['LINKPATH'] = ""
		env['LINKFILE'] = ""

	# Loop through Drivers
	driver_list = []
	type_id_headers = []
	drivers = dev.getDriverList(device, env['XPCC_PLATFORM_PATH'])
	for driver in drivers:
		ddic = {} # create dictionary describing the driver
		d = DriverFile.fromDict(driver, env['XPCC_PARAMETER_DB'], env.GetLogger())
		ddic['name'] = d.name
		ddic['type'] = d.type
		ddic['headers'] = []
		build = d.getBuildList(env['XPCC_PLATFORM_PATH'], env['XPCC_DEVICE'])
		for f in build:
			src = os.path.join(platform_path, f[0])
			tar = os.path.join(generated_path, f[1])
			if len(f) == 3:
				res = env.Jinja2Template(target = tar, source = src, substitutions = f[2])
			else:
				res = env.Command(tar, src, Copy("$TARGET", "$SOURCE"))
			# check if target is header file
			if os.path.splitext(tar)[1] in Scanner.HEADER:
				if not f[1].endswith("_impl.hpp"):
					ddic['headers'].append(f[1]) # append path relative to platform dir
				includes.append(os.path.dirname(tar))
			# or source file
			elif os.path.splitext(tar)[1] in Scanner.SOURCE:
				sources.append(res)
			# check if target is "type_ids.hpp"
			if os.path.basename(tar) == "type_ids.hpp":
				type_id_headers.append(f[1]) # append path relative to platform dir
		driver_list.append(ddic)

	####### Generate Header Templates #########################################
	# Show SCons how to build the architecture/platform.hpp file:
	src = os.path.join(platform_path, 'platform.hpp.in')
	tar = os.path.join(architecture_path, 'platform.hpp')
	platform_include_path = 'generated_platform_' + device + '/drivers.hpp'
	# Check if architecture/platform.hpp already exists
	if os.path.exists(tar):
		f = open(tar, 'r')
		content = f.read()
		f.close()
		# Check if architecture/platform.hpp file points to the correct directory
		if platform_include_path not in content:
			# if not, remove in order to use the correct generated directory
			os.remove(tar)
	sub = {'include_path': platform_include_path}
	env.Template(target = tar, source = src, substitutions = sub)
	# Show SCons how to build the drivers.hpp.in file:
	src = os.path.join(platform_path, 'drivers.hpp.in')
	tar = os.path.join(generated_path, 'drivers.hpp')
	sub = {'drivers': driver_list}
	env.Jinja2Template(target = tar, source = src, substitutions = sub)
	# Show SCons how to build device.hpp.in file:
	src = os.path.join(platform_path, 'device.hpp.in')
	tar = os.path.join(generated_path, 'device.hpp')
	sub = {'headers': device_headers}
	env.Jinja2Template(target = tar, source = src, substitutions = sub)
	# Show SCons how to build type_ids.hpp.in file:
	src = os.path.join(platform_path, 'type_ids.hpp.in')
	tar = os.path.join(generated_path, 'type_ids.hpp')
	sub = {'headers': type_id_headers}
	env.Jinja2Template(target = tar, source = src, substitutions = sub)
	return sources, defines, includes

############## Template Tests #################################################
# -----------------------------------------------------------------------------
def test_item(dic, item_key, item_value, starts_with=False):
	if item_key not in dic:
		return False
	if starts_with and dic[item_key].startswith(item_value):
		return True
	if not starts_with and dic[item_key] == item_value:
		return True
	return False

############## Template Filters ###############################################
# -----------------------------------------------------------------------------
def filter_get_ports(gpios):
	"""
	This filter accepts a list of gpios as e.g. used by the stm32af driver
	and tried to extract information about port which is returned as a list
	of dictionaries with the following strcture:
	{'name': "C", 'startPin': 0, 'width': 16}
	"""
	# collect information on available gpios
	port_ids = {}
	for gpio in gpios:
		if not gpio['port'] in port_ids:
			port_ids[gpio['port']] = [0] * 16
		port_ids[gpio['port']][int(gpio['id'])] = 1
	# create port list
	ports = []
	for name, ids in port_ids.iteritems():
		# if the port consists of at least one gpio pin
		if 1 in ids:
			port = {}
			port['name'] = name
			# find start pin as well as width
			ii = ids.index(1)
			port['startPin'] = ii
			while ii < 16 and ids[ii] == 1:
				ii = ii + 1
			port['width'] = ii - port['startPin']
			ports.append(port)
	return ports

# -----------------------------------------------------------------------------
###################### Generate Platform Tools ################################
def generate(env, **kw):
	# Set some paths used by this file
	env['XPCC_PLATFORM_PATH'] = \
		os.path.join(env['XPCC_LIBRARY_PATH'], 'xpcc', 'architecture', 'platform')
	env['XPCC_PLATFORM_GENERATED_DIR'] = 'generated_platform_' + env['XPCC_DEVICE']
	env['XPCC_PLATFORM_GENERATED_PATH'] = \
		os.path.join(env['XPCC_LIBRARY_PATH'], 'xpcc', 'architecture', env['XPCC_PLATFORM_GENERATED_DIR'])

	# Create Parameter DB and parse User parameters
	env['XPCC_PARAMETER_DB'] = ParameterDB(env['XPCC_USER_PARAMETERS'], env.GetLogger()).toDictionary()

	# Remove Generated Folder if Clean Flag is set
	# Scons does not remove the files on its own
	if env.GetOption('clean'):
		env.Info("Removing %s..." % env['XPCC_PLATFORM_GENERATED_PATH'])
		Execute(Delete(env['XPCC_PLATFORM_GENERATED_PATH']))

	# Add Method to Parse XML Files, and create Template / Copy Dependencies
	env.AddMethod(platform_tools_generate, 'GeneratePlatform')
	env.AddMethod(platform_tools_find_device_file, 'FindDeviceFile')

	# Add Filter for Gpio Drivers to Template Engine
	env.AddTemplateJinja2Filter('getPorts', filter_get_ports)

	########## Add Template Tests #############################################
	# Generaic Tests (they accept a string)
	def test_platform(target, platform):
		return test_item(target, 'platform', platform)
	env.AddTemplateJinja2Test('platform', test_platform)
	def test_family(target, family):
		return test_item(target, 'family', family)
	env.AddTemplateJinja2Test('family', test_family)
	def test_name(target, name):
		return test_item(target, 'name', name)
	env.AddTemplateJinja2Test('name', test_name)
	def test_type(target, type):
		return test_item(target, 'type', type)
	env.AddTemplateJinja2Test('type', test_type)
	def test_size_id(target, size_id):
		return test_item(target, 'size-id', size_id)
	env.AddTemplateJinja2Test('size_id', test_size_id)
	def test_pin_id(target, pin_id):
		return test_item(target, 'pin-id', pin_id)
	env.AddTemplateJinja2Test('pin_id', test_pin_id)
	def test_core(target, core, starts_with=False):
		return test_item(target, 'core', core, starts_with)
	env.AddTemplateJinja2Test('core', test_core)

	# Core Tests
	def test_cortex(target):
		return test_core(target, 'cortex', True)
	env.AddTemplateJinja2Test('cortex', test_cortex)
	def test_cortex_m0(target):
		return test_core(target, 'cortex-m0')
	env.AddTemplateJinja2Test('cortex_m0', test_cortex_m0)
	def test_cortex_m3(target):
		return test_core(target, 'cortex-m3')
	env.AddTemplateJinja2Test('cortex_m3', test_cortex_m3)
	def test_cortex_m4(target):
		return test_core(target, 'cortex-m4', True)
	env.AddTemplateJinja2Test('cortex_m4', test_cortex_m4)
	def test_cortex_m4f(target):
		return test_core(target, 'cortex-m4f')
	env.AddTemplateJinja2Test('cortex_m4f', test_cortex_m4f)

	# Platform Tests
	def test_is_stm32(target):
		return test_platform(target, 'stm32')
	env.AddTemplateJinja2Test('stm32', test_is_stm32)
	def test_is_lpc(target):
		return test_platform(target, 'lpc')
	env.AddTemplateJinja2Test('lpc', test_is_lpc)
	def test_is_avr(target):
		return test_platform(target, 'avr')
	env.AddTemplateJinja2Test('avr', test_is_avr)

	# STM32 Family Test
	def test_is_stm32f0(target):
		return test_platform(target, 'stm32') and test_family(target, 'f0')
	env.AddTemplateJinja2Test('stm32f0', test_is_stm32f0)
	def test_is_stm32f1(target):
		return test_platform(target, 'stm32') and test_family(target, 'f1')
	env.AddTemplateJinja2Test('stm32f1', test_is_stm32f1)
	def test_is_stm32f2(target):
		return test_platform(target, 'stm32') and test_family(target, 'f2')
	env.AddTemplateJinja2Test('stm32f2', test_is_stm32f2)
	def test_is_stm32f3(target):
		return test_platform(target, 'stm32') and test_family(target, 'f3')
	env.AddTemplateJinja2Test('stm32f3', test_is_stm32f3)
	def test_is_stm32f4(target):
		return test_platform(target, 'stm32') and test_family(target, 'f4')
	env.AddTemplateJinja2Test('stm32f4', test_is_stm32f4)

# -----------------------------------------------------------------------------
def exists(env):
	return True

#!/usr/bin/env python
# 
# Copyright (c) 2011, Roboterclub Aachen e.V.
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
# 
# DESCRIPTION
# 
# This is a tool to compile programs for ARM7 and Cortex-M3 microcontrollers.

import os
from SCons.Script import *

import arm_devices as device_list

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.Append(ENV = {'PATH' : os.environ['PATH']})
	env.Tool('gcc')
	env.Tool('g++')
	env.Tool('gnulink')
	env.Tool('ar')
	env.Tool('as')
	
	env['PROGSUFFIX'] = ".elf"
	
	# used programs
	env['CC'] = "arm-none-eabi-gcc"
	env['CXX'] = "arm-none-eabi-g++"
	env['AS'] = "arm-none-eabi-as"
	env['OBJCOPY'] = "arm-none-eabi-objcopy"
	env['OBJDUMP'] = "arm-none-eabi-objdump"
	env['AR'] = "arm-none-eabi-ar"
	env['NM'] = "arm-none-eabi-nm"
	env['RANLIB'] = "arm-none-eabi-ranlib"
	env['SIZE'] = "arm-none-eabi-size"
	
	# build messages
	if ARGUMENTS.get('verbose') != '1':
		env['CCCOMSTR'] = "Compiling C: $TARGET"
		env['CXXCOMSTR'] = "Compiling C++: $TARGET"
		env['ASCOMSTR'] = "Assembling: $TARGET"
		env['ASPPCOMSTR'] = "Assembling: $TARGET"
		env['LINKCOMSTR'] = "Linking: $TARGET"
		env['RANLIBCOMSTR'] = "Indexing: $TARGET"
		env['ARCOMSTR'] = "Create Library: $TARGET"
		
		env['SIZECOMSTR'] = "Size after:"
		env['SYMBOLSCOMSTR'] = "Show symbols for '$SOURCE':"
		env['HEXCOMSTR'] = "Creating load file for Flash: $TARGET"
		env['BINCOMSTR'] = "Creating load file for Flash: $TARGET"
		env['LSSCOMSTR'] = "Creating Extended Listing: $TARGET"
	
	# import device specific settings
	device = env['ARM_DEVICE']
	if device.startswith('stm32f'):
		# The first character after the number specifies the package. As this
		# is not important for the memory map of the device we replace it with
		# an underscore when selecting the linkerscript.
		device = device[0:9] + '_' + device[10:] 
	
	try:
		defines = device_list.devices[device]['defines']
		linkerscript = device_list.devices[device]['linkerscript']
		
		# For the env.Size() command. See helper.py
		env['DEVICE_SIZE'] = device_list.devices[device]['size']
	except KeyError as e:
		print "Unknown device '%s'. Please check the spelling or add the device " \
				"in scons/arm_devices.py" % env['ARM_DEVICE']
	
	# Add a empty dict to define the internal type of CPPDEFINES. This way
	# we can later add lists or dicts. When starting with a list only other
	# lists will work afterwards.
	env.Append(CPPDEFINES = {})
	env.Append(CPPDEFINES = ["__ARM_%s__" % env['ARM_DEVICE'].upper()])
	env.Append(CPPDEFINES = defines)
	
	# If nothing else has been selected enable Thumb-Mode for AT91SAM devices
	# as it will be most likely faster.
	# Can be disabled by putting 'env['THUMB'] = ""' in the SConstruct file 
	if not env.has_key('THUMB') and env['ARM_DEVICE'].startswith('at91sam7'):
		env['THUMB'] = '-mthumb'
	
	# Cortex-M3/0 suppports only the Thumb-2 instruction set 
	if env['ARM_ARCH'].startswith('cortex-m'):
		env['THUMB'] = '-mthumb'
		env['THUMB_ASSEMBLER'] = '-mthumb'
		# Important to let the linker choose thumb implementations of
		# predefined functions from the standard libraries.
		# (for example __libc_init_array())
		env['THUMB_LINKER'] = '-mthumb'
	
	if env['ARM_ARCH'] is 'cortex-m4':
		env['FPU'] = '-mfloat-abi=softfp -mfpu=fpv4-sp-d16'
	
	# C flags
	env['CFLAGS'] = [
		"-std=gnu99",
		"-Wstrict-prototypes",
		"-Wredundant-decls",
		"-Wnested-externs",
	]
	
	# flags for C and C++
	env['CCFLAGS'] = [
		"-mcpu=$ARM_ARCH",
		"$THUMB",			# use THUMB='-mthumb' to compile as thumb code (default for AT91SAM)
		"$FPU",
		"-mthumb-interwork",
		"-Os",
		"-gdwarf-2",
		"-funsigned-char",
		"-funsigned-bitfields",
		"-fshort-enums",
		"-ffunction-sections",
		"-fdata-sections",
		"-fno-split-wide-types",
		"-fno-move-loop-invariants",
		"-fno-tree-loop-optimize",
		"-fno-unwind-tables",
		"-mlong-calls",		# when using ".fastcode" without longcall:
		"-Wall",
		"-Wformat",
		"-Wextra",
		"-Wundef",
		"-Winit-self",
#		"-Wcast-qual",		# TODO enable this
#		"-Wimplicit",
#		"-Wshadow",			# TODO enable this
		"-Wpointer-arith",
		"-Wunused",
		"-Wa,-adhlns=${TARGET.base}.lst",
		"-DBASENAME=${SOURCE.file}",
	]
	
	# C++ flags
	env['CXXFLAGS'] = [
#		"-fverbose-asm",
#		"-save-temps",		# save preprocessed files
		"-std=gnu++98",
		"-fno-exceptions",
		"-fno-rtti",
		"-fno-threadsafe-statics",
		"-fuse-cxa-atexit",
		"-nostdlib",
		"-Woverloaded-virtual",
#		"-Wnon-virtual-dtor",
#		"-Weffc++",
	]
	
	# Assembler flags
	env['ASFLAGS'] = [
		"-mcpu=$ARM_ARCH",
		"-mthumb-interwork",
		"$THUMB_ASSEMBLER",
		"-gdwarf-2",
		"-xassembler-with-cpp",
		"-Wa,-adhlns=${TARGET.base}.lst",
		"-D__ASSEMBLY__",		# for at91lib
	]
	
	linkdir, linkfile = os.path.split(linkerscript)
	env['LINKFLAGS'] = [
		"-mcpu=$ARM_ARCH",
		"$THUMB_LINKER",
		"-Wl,--relax",
		"-Wl,--gc-sections",
		"-nostartfiles",
		"-L${XPCC_ROOTPATH}/src/xpcc/architecture/platform/%s" % linkdir,
		"-T%s" % linkfile,
		"-Wl,-Map=${TARGET.base}.map,--cref",
	]
	
	clock = str(env['ARM_CLOCK']).lower()
	env.Append(CPPDEFINES = {'F_CPU' : clock })
	
	builder_hex = Builder(
		action = Action("$OBJCOPY -O ihex -R .eeprom $SOURCE $TARGET",
						cmdstr = "$HEXCOMSTR"), 
		suffix = ".hex", 
		src_suffix = ".elf")
	
	builder_bin = Builder(
		action = Action("$OBJCOPY -O binary -R .eeprom $SOURCE $TARGET",
						cmdstr = "$BINCOMSTR"), 
		suffix = ".bin", 
		src_suffix = ".elf")
	
	builder_listing = Builder(
		action = Action("$OBJDUMP -h -S $SOURCE > $TARGET",
						cmdstr = "$LSSCOMSTR"), 
		suffix = ".lss", 
		src_suffix = ".elf")
	
	env.Append(BUILDERS = {
		'Hex': builder_hex,
		'Bin': builder_bin,
		'Listing': builder_listing,
	})

	# changes env['ARCOM'] and env['LINKCOM']
	if str(Platform()) == "win32":
		env.Tool('gcc_windows')
		
# -----------------------------------------------------------------------------
def exists(env):
	return env.Detect('arm-none-eabi-gcc')

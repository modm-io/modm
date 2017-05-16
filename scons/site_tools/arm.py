#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2009, 2011, 2017, Georgi Grinshpun
# Copyright (c) 2012, 2014, 2016, Niklas Hauser
# Copyright (c) 2012, 2015-2016, Sascha Schade
# Copyright (c) 2013, 2015, Kevin Läufer
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2016, Raphael Lehmann
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------
# 
# DESCRIPTION
# 
# This is a tool to compile programs for ARM7, Cortex-M0 and Cortex-M3 microcontrollers.

import os
from SCons.Script import *

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

	# Add a empty dict to define the internal type of CPPDEFINES. This way
	# we can later add lists or dicts. When starting with a list only other
	# lists will work afterwards.
	env.Append(CPPDEFINES = {})
	env.Append(CPPDEFINES = ["__ARM_%s__" % env['ARM_DEVICE'].upper()])
	
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

	# Single precision float support
	if env['ARCHITECTURE'] == 'cortex-m4f':
		# Options for '-mfloat-abi='
		# - soft: Full software floating point.
		# - softfp: Use the FPU, but remain compatible with soft-float code.
		# - hard: Full hardware floating point. Needs support from libc. 
		env['FPU'] = '-mfloat-abi=softfp -mfpu=fpv4-sp-d16'

	# Single precision float support
	# PM0253 4.7: "The Cortex-M7 Floating-Point Unit (FPU) implements the FPv5 floating-point extensions."
	if env['ARCHITECTURE'] == 'cortex-m7f':
		env['FPU'] = '-mfloat-abi=softfp -mfpu=fpv5-sp-d16'

	# Double precision float support
	if env['ARCHITECTURE'] == 'cortex-m7fd':
		env['FPU'] = '-mfloat-abi=softfp -mfpu=fpv5-d16'
	
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
		"$FPU",
		"$THUMB",			# use THUMB='-mthumb' to compile as thumb code (default for AT91SAM)
		"-mthumb-interwork",
		"$OPTIMIZATION",
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
		"-finline-limit=10000",
		"-mlong-calls",		# when using ".fastcode" without longcall
		"-Wall",
		"-Werror=maybe-uninitialized",
		"-Wformat",
		"-Wextra",
		"-Wundef",
		"-Winit-self",
#		"-Wcast-qual",		# TODO enable this
#		"-Wimplicit",
#		"-Wshadow",			# TODO enable this
		"-Wpointer-arith",
		"-Wunused",
#		"-Wa,-adhlns=${TARGET.base}.lst",
		"-DBASENAME=${SOURCE.file}",
		"-fdiagnostics-color=auto",
	]
	env['CCFLAGS'] += env['MODM_ADDITIONAL_CCFLAGS']

	# Platforms with single precision float support
	if env['ARCHITECTURE'] in ['cortex-m4f', 'cortex-m7f']:
		env['CFLAGS'] += [
			# All constants are assumed to be float (32-bit) and not
			# double (64-bit) by default.
			"-fsingle-precision-constant",
			# Warn if a float value is implicit promoted to double. Doubles are
			# emulated in software while floats can use the FPU.
			"-Wdouble-promotion",
		]

	# C++ flags
	env['CXXFLAGS'] = [
#		"-fverbose-asm",
#		"-save-temps",		# save preprocessed files
		"-std=c++11",
		"-fconstexpr-depth=10000", # enable deep recursion
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
		"$FPU",
		"-mthumb-interwork",
		"$THUMB_ASSEMBLER",
		"-gdwarf-2",
		"-xassembler-with-cpp",
		"-Wa,-adhlns=${TARGET.base}.lst",
		"-D__ASSEMBLY__",		# for at91lib
	]
	
	# these values can be overwritten by the user
	# now set in platform_tools.py
	# linkdir, linkfile = os.path.split(linkerscript)
	# env['LINKPATH'] = "${MODM_ROOTPATH}/src/modm/architecture/platform/%s" % linkdir
	# env['LINKFILE'] = "%s" % linkfile
	
	env['LINKFLAGS'] = [
		"-mcpu=$ARM_ARCH",
		"$THUMB_LINKER",
		"-Wl,--relax",
		"-Wl,--gc-sections",
		"-Wl,-wrap,_malloc_r",
		"-Wl,-wrap,_calloc_r",
		"-Wl,-wrap,_realloc_r",
		"-Wl,-wrap,_free_r",
		"--specs=nosys.specs",
		"--specs=nano.specs",
		"-nostartfiles",
		"-L$LINKPATH",
		"-T$LINKFILE",
		"-Wl,--fatal-warnings",
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
		action = Action("$OBJDUMP -hSlt $SOURCE > $TARGET",
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

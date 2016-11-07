#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2009, 2011, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, Niklas Hauser
# Copyright (c) 2016, Daniel Krebs
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
# This is a tool compile programs for the AVR microcontrollers from Atmel.
# The 'avr-gcc' is used as compiler.
# 
# EXAMPLE
# 
# A simple SConstruct file to compile a file named 'main.c' for an ATmega644
# can look as follows:
# 
#    env = Environment(tools = ['avr'],
#             AVR_DEVICE = 'atmega644',
#             AVR_CLOCK = 16000000)
#    
#    program = env.Program('main.c')
#    
#    env.Hex(program)
#    env.Listing(program)
#
# BUILDERS
# 
# The following builders are available:
# 
# Object()      .c   -> .o
# Object()      .cpp -> .o
# Object()      .S   -> .o
# Library()     .o   -> .a
# Program()     .o   -> .elf
# Hex()         .elf -> .hex
# Eeprom()      .elf -> .eep
# Listing()     .elf -> .lss
#
# The 'Program' builder will automatically call the 'Object' builder so
# it's no need to use it explicitly. Just call 'Program' with all your
# source files.
#
# TODO
# 
# - Make the optimization level adjustable

import os
from SCons.Script import *
import SCons.Subst
import tempfile
import os

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
	env['CC'] = "avr-gcc"
	env['CXX'] = "avr-g++"
	env['AS'] = "avr-as"
	env['OBJCOPY'] = "avr-objcopy"
	env['OBJDUMP'] = "avr-objdump"
	env['AR'] = "avr-ar"
	env['NM'] = "avr-nm"
	env['RANLIB'] = "avr-ranlib"
	env['SIZE'] = "avr-size --mcu=$AVR_DEVICE --format=avr"
	
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
		env['EEPROMCOMSTR'] = "Creating load file for EEPROM: $TARGET"
		env['LSSCOMSTR'] = "Creating Extended Listing: $TARGET"
	
	# C flags
	env['CFLAGS'] = [
		"-std=gnu99",
		"-Wstrict-prototypes",
		"-fpack-struct",
	]
	
	# flags for C and C++
	env['CCFLAGS'] = [
		"-mmcu=$AVR_DEVICE", 
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
		"-finline-limit=10000",
		"-fverbose-asm",	# TODO check this
		"-Wall",
		"-Wextra",
		"-Wundef",
#		"-Winline",
		"-Wa,-adhlns=${TARGET.base}.lst",
		"-DBASENAME=${SOURCE.file}",
	]
	env['CCFLAGS'] += env['XPCC_ADDITIONAL_CCFLAGS']
	
	# C++ flags
	env['CXXFLAGS'] = [
#		"-std=gnu++98",
		"-std=gnu++0x",
		"-fno-exceptions", 
		"-fno-rtti",
		"-fno-threadsafe-statics",
		"-fuse-cxa-atexit",
		"-nostdlib",
		"-Woverloaded-virtual",
#		"-Wnon-virtual-dtor",
#		"-Weffc++",
#		"-save-temps",		# save preprocessed files
	]
	
	# Assembler flags
	env['ASFLAGS'] = [
		"-mmcu=$AVR_DEVICE",
		"-xassembler-with-cpp",
	]
	
	env['LINKFLAGS'] = [
		"-mmcu=$AVR_DEVICE", 
		"-Wl,--relax", 
		"-Wl,--gc-sections",
		"-Wl,-T,${XPCC_ROOTPATH}/src/xpcc/architecture/platform/driver/core/avr/linkerscript.ld",
#		"-Wl,-Map=${TARGET.base}.map,--cref", 
#		"-Wl,-u,vfprintf -lprintf_flt"		# enable float support for vfprinft
	]
	
	env['LINKCOM'] = "$LINK -o $TARGET $LINKFLAGS -lm -lc -lm $SOURCES -lm $_LIBDIRFLAGS $_LIBFLAGS -lm"

	clock = str(env['AVR_CLOCK']).lower()
	if not clock.endswith('ul'):
		clock += 'ul'
	env.Append(CPPDEFINES = {'F_CPU' : clock})
	
	builder_hex = Builder(
		action = Action("$OBJCOPY -O ihex -R .eeprom $SOURCE $TARGET",
						cmdstr = "$HEXCOMSTR"), 
		suffix = ".hex", 
		src_suffix = ".elf")

	builder_eeprom = Builder(
		action = Action("$OBJCOPY -j .eeprom --set-section-flags=.eeprom=\"alloc,load\" --change-section-lma .eeprom=0 --no-change-warnings --change-section-lma .eeprom=0 -O ihex $SOURCE $TARGET",
						cmdstr = "$EEPROMCOMSTR"), 
		suffix = ".eep",
		src_suffix = ".elf")

	builder_listing = Builder(
		action = Action("$OBJDUMP -h -S $SOURCE > $TARGET",
						cmdstr = "$LSSCOMSTR"), 
		suffix = ".lss", 
		src_suffix = ".elf")
	
	env.Append(BUILDERS = {
		'Hex': builder_hex,
		'Eeprom': builder_eeprom,
		'Listing': builder_listing,
	})

	# changes env['ARCOM'] and env['LINKCOM']
	if str(Platform()) == "win32":
		env.Tool('gcc_windows')

# -----------------------------------------------------------------------------
def exists(env):
	return env.Detect('avr-gcc')

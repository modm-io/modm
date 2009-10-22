#!/usr/bin/env python
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
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

from SCons.Script import *

# c -> o			build-in
# cpp -> o			build-in
# S -> o			?
# o -> a			build-in
# o -> elf			ok
# elf -> hex		ok
# elf -> eep		?
# elf -> lss		ok

# -----------------------------------------------------------------------------
def show_size(env, source):
	return [env.Command(None,
						source,
						Action("$SIZE --mcu=$AVR_DEVICE --format=avr $SOURCE", 
								cmdstr="$SIZECOMSTR"))]

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.Append(ENV = {'PATH' : os.environ['PATH']})
	env.Tool('gcc')
	env.Tool('g++')
	env.Tool('gnulink')
	env.Tool('ar')
	env.Tool('as')
	
	env['PROGSUFFIX'] = ".elf"
	
	# used tools
	env['CC'] = "avr-gcc"
	env['CXX'] = "avr-g++"
	env['AS'] = "avr-as"
	env['OBJCOPY'] = "avr-objcopy"
	env['OBJDUMP'] = "avr-objdump"
	env['AR'] = "avr-ar"
	env['NM'] = "avr-nm"
	env['RANLIB'] = "avr-ranlib"
	env['SIZE'] = "avr-size"
	
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
		env['HEXCOMSTR'] = "Creating load file for Flash: $TARGET"
		env['LSSCOMSTR'] = "Creating Extended Listing: $TARGET"
	
	# C flags
	env['CFLAGS'] = ' '.join([
		"-std=gnu99",
		"-Wstrict-prototypes",
	])
	
	# flags for C and C++
	env['CCFLAGS'] = " ".join([
		"-mmcu=$AVR_DEVICE", 
		"-Os",
		"-gdwarf-2", 
		"-funsigned-char",
		"-funsigned-bitfields", 
		"-fpack-struct", 		# TODO
		"-fshort-enums", 
		"-ffunction-sections",
		"-fdata-sections", 
		"-Wall",
		"-Wextra",
		"-Wundef", 
		"-Wa,-adhlns=${TARGET.base}.lst",
	])
	
	# C++ flags
	env['CXXFILESUFFIX'] = ".cpp"
	env['CXXFLAGS'] = ' '.join([
		"-std=gnu++98",
		"-fno-exceptions", 
		"-fno-rtti",
		"-fno-threadsafe-statics",
		"-fuse-cxa-atexit",
		"-nostdlib",
#		"-Wnon-virtual-dtor",
#		"-Weffc++",
		"-Woverloaded-virtual",
	])
	
	# Assembler flags
	env['ASFLAGS'] = "-mmcu=$AVR_DEVICE -x assembler-with-cpp "
	
	env['LINKFLAGS'] = " ".join([
		"-mmcu=$AVR_DEVICE", 
		"-Wl,-Map=${TARGET.base}.map,--cref", 
		"-Wl,--relax", 
		"-Wl,--gc-sections", 
#		"-static", 
	])
	
	env['LINKCOM'] = "$LINK -o $TARGET $LINKFLAGS $SOURCES -lm $_LIBDIRFLAGS $_LIBFLAGS -lm"
	
	clock = str(env['AVR_CLOCK']).lower()
	if not clock.endswith('ul'):
		clock += 'ul'
	env.Append(CPPDEFINES = {'F_CPU' : clock})
	
	env.AddMethod(show_size, 'Size')
	builder_hex = Builder(action=Action("$OBJCOPY -O ihex -R .eeprom $SOURCE $TARGET",
											cmdstr="$HEXCOMSTR"), 
								suffix=".hex", 
								src_suffix=".elf")
	builder_listing = Builder(action=Action("$OBJDUMP -h -S $SOURCE > $TARGET", 
												cmdstr="$LSSCOMSTR"), 
								suffix=".lss", 
								src_suffix=".elf")
	
	env.Append(BUILDERS = {
		'Hex': builder_hex,
		'Listing': builder_listing,
	})

# -----------------------------------------------------------------------------
def exists(env):
	return env.Detect('avr-gcc')

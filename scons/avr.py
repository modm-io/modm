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
#               .elf -> .eep
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
# We use an adapted Version of this class from 'SCons/Platform/__init__.py' for
# Windows because GCC requires all backslashes inside a paramter file to be escaped.
class TempFileMungeWindows(object):
	def __init__(self, cmd):
		self.cmd = cmd
	
	def __call__(self, target, source, env, for_signature):
		if for_signature:
			return self.cmd
		
		# do the expansion.
		cmd = env.subst_list(self.cmd, SCons.Subst.SUBST_CMD, target, source)[0]
		
		# create a file for the arguments
		(fd, tmp) = tempfile.mkstemp('.lnk', text=True)
		native_tmp = SCons.Util.get_native_path(os.path.normpath(tmp))
		
		args = list(map(SCons.Subst.quote_spaces, cmd[1:]))
		output = " ".join(args).replace("\\", "\\\\")
		os.write(fd, output + "\n")
		os.close(fd)
		return [cmd[0], '@' + native_tmp + '\nrm', native_tmp]

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
		"-Wall",
		"-Wextra",
		"-Wundef", 
		"-Wa,-adhlns=${TARGET.base}.lst",
		"-DBASENAME=${SOURCE.file}",
	]
	
	# C++ flags
	env['CXXFLAGS'] = [
		"-std=gnu++98",
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
	if str(Platform()) == "win32":
		env['ASFLAGS'] = ["-mmcu=$AVR_DEVICE"]
	else:
		env['ASFLAGS'] = ["-mmcu=$AVR_DEVICE", "-xassembler-with-cpp"]
	
	env['LINKFLAGS'] = [
		"-mmcu=$AVR_DEVICE", 
		"-Wl,--relax", 
		"-Wl,--gc-sections",
#		"-Wl,-Map=${TARGET.base}.map,--cref", 
#		"-Wl,-u,vfprintf -lprintf_flt"		# enable float support for vfprinft
	]
	
	env['LINKCOM'] = "$LINK -o $TARGET $LINKFLAGS -lm -lc -lm $SOURCES -lm $_LIBDIRFLAGS $_LIBFLAGS -lm"
	
	if str(Platform()) == "win32":
		# use a tempfile for the arguments, otherwise the command line string might be to long
		# for windows to handle (maximum length is 2048 characters)
		env['TEMPFILE'] = TempFileMungeWindows
		env['LINKCOM'] = "${TEMPFILE('%s')}" % env['LINKCOM']
		env['ARCOM'] = "${TEMPFILE('%s')}" % env['ARCOM']
	
	clock = str(env['AVR_CLOCK']).lower()
	if not clock.endswith('ul'):
		clock += 'ul'
	env.Append(CPPDEFINES = {'F_CPU' : clock})
	
	builder_hex = Builder(
		action = Action("$OBJCOPY -O ihex -R .eeprom $SOURCE $TARGET",
						cmdstr = "$HEXCOMSTR"), 
		suffix = ".hex", 
		src_suffix = ".elf")
	
	builder_listing = Builder(
		action = Action("$OBJDUMP -h -S $SOURCE > $TARGET",
						cmdstr = "$LSSCOMSTR"), 
		suffix = ".lss", 
		src_suffix = ".elf")
	
	env.Append(BUILDERS = {
		'Hex': builder_hex,
		'Listing': builder_listing,
	})

# -----------------------------------------------------------------------------
def exists(env):
	return env.Detect('avr-gcc')

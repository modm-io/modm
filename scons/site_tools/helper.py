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

from SCons.Script import *

import re
import subprocess

# -----------------------------------------------------------------------------
# Output of 'arm-none-eabi-size -A build/stm32_p103.elf':
# build/stm32_p103.elf  :
# section             size        addr
# .reset               236   134217728
# .fastcode             48   536870912
# .text              18220   134218016
# .rodata             4276   134236240
# .data               1336   536870960
# .bss                2688   536872296
# .stack               640   536874984
# .comment              42           0
# .debug_aranges      3360           0
# (...)
# Total             285915
# 
# Try to match the lines (name, size, address) to get the size of the
# individual regions
filter = re.compile('^(?P<section>[.]\w+)\s*(?P<size>\d+)\s*(?P<addr>\d+)$')

# Sections which will remain in the Flash
flashSectionNames = ['.reset', '.fastcode', '.fastdata', '.text', '.rodata', '.data']

# Sections which will be created in RAM or are copied from the Flash. In that
# case the section will appear also in `flashSectionNames`.
ramSectionNames = ['.vectors', '.fastcode', '.fastdata', '.data', '.bss', '.noinit']

def size_action(target, source, env):
	cmd = [env['SIZE'], '-A', str(source[0])]
	
	# Run the default nm command (`arm-none-eabi-nm` in this case)
	p = subprocess.Popen(cmd, stdout=subprocess.PIPE)
	stdout, stderr = p.communicate()
	
	if stderr is not None:
		env.Error("Error while running %s" % ' '.join(cmd))
		Exit(1)
	
	flashSize = 0
	ramSize = 0
	flashSections = {}
	ramSections = {}
	for line in stdout.splitlines():
		match = filter.match(line)
		if match:
			section = match.group('section')
			if section in flashSectionNames:
				flashSize += int(match.group('size'))
				flashSections[section] = 1
			if section in ramSectionNames:
				ramSize += int(match.group('size'))
				ramSections[section] = 1
	
	# create lists of the used sections for Flash and RAM
	flashSections = flashSections.keys()
	flashSections.sort()
	ramSections = ramSections.keys()
	ramSections.sort()
	
	flashPercentage = flashSize / float(env['DEVICE_SIZE']['flash']) * 100.0
	ramPercentage = ramSize / float(env['DEVICE_SIZE']['ram']) * 100.0
	
	device = env['ARM_DEVICE']
	
	print """Memory Usage
------------
Device: %s

Program: %7d bytes (%2.1f%% Full)
(%s)

Data:    %7d bytes (%2.1f%% Full)
(%s)
""" % (device, flashSize, flashPercentage, ' + '.join(flashSections), \
	ramSize, ramPercentage, ' + '.join(ramSections))

# -----------------------------------------------------------------------------
def show_size(env, source, alias='__size'):
	if env['ARCHITECTURE'] == 'avr8':
		# use the raw output of the size tool
		action = Action("$SIZE %s" % source[0].path, 
					cmdstr="$SIZECOMSTR")
	else:
		action = Action(size_action, cmdstr="$SIZECOMSTR")

	return env.AlwaysBuild(env.Alias(alias, source, action))

def list_symbols(env, source, alias='__symbols'):
	action = Action("$NM %s -S -C --size-sort -td" % source[0].path, 
					cmdstr="$SYMBOLSCOMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

def run_program(env, program):
	return env.Command('thisfileshouldnotexist', program, '@"%s"' % program[0].abspath)

def phony_target(env, **kw):
	for target, action in kw.items():
		env.AlwaysBuild(env.Alias(target, [], action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	if ARGUMENTS.get('verbose') != '1':
		env['SIZECOMSTR'] = "Size after:"
		env['SYMBOLSCOMSTR'] = "Show symbols for '$SOURCE':"
	
	env.AddMethod(show_size, 'Size')
	env.AddMethod(list_symbols, 'Symbols')
	
	env.AddMethod(run_program, 'Run')
	env.AddMethod(phony_target, 'Phony')

def exists(env):
	return True

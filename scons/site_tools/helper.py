#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2015-2016, Niklas Hauser
# Copyright (c) 2013, Kevin Läufer
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

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
	stackSize = 0
	heapSize = 0
	flashSections = {}
	ramSections = {}
	stackSections = {}
	heapSections = {}

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
			if section.startswith('.stack'):
				stackSize += int(match.group('size'))
				stackSections[section] = 1
			if section.startswith('.heap'):
				heapSize += int(match.group('size'))
				heapSections[section] = 1

	# create lists of the used sections for Flash and RAM
	flashSections = flashSections.keys()
	flashSections.sort()
	ramSections = ramSections.keys() + stackSections.keys()
	ramSections.sort()
	heapSections = heapSections.keys()
	heapSections.sort()

	flashPercentage = flashSize / float(env['DEVICE_SIZE']['flash']) * 100.0
	ramPercentage = ramSize / float(env['DEVICE_SIZE']['ram']) * 100.0
	stackPercentage = stackSize / float(env['DEVICE_SIZE']['ram']) * 100.0
	heapPercentage = heapSize / float(env['DEVICE_SIZE']['ram']) * 100.0

	device = env['ARM_DEVICE']

	print """Memory Usage
------------
Device: %s

Program: %7d bytes (%2.1f%% used)
(%s)

Data:    %7d bytes (%2.1f%% used) = %d bytes static (%2.1f%%) + %d bytes stack (%2.1f%%)
(%s)

Heap:    %7d bytes (%2.1f%% available)
(%s)
""" % (device, flashSize, flashPercentage, ' + '.join(flashSections),
	   ramSize + stackSize, ramPercentage + stackPercentage, ramSize, ramPercentage, stackSize, stackPercentage, ' + '.join(ramSections),
	   heapSize, heapPercentage, ' + '.join(heapSections))

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

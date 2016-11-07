#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Sascha Schade
# Copyright (c) 2014, 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------
#
# Usage:
# Add to your SConstruct file
# binfile = env.Bin(program)
# env.Alias('program', env.DfuStm32Programmer(binfile))
# 
# Call
#   scons program
# from your command line

import platform
from SCons.Script import *

# -----------------------------------------------------------------------------
def dfu_stm32_programmer_flash(env, source, alias='dfu_stm32_program'):
	# dfu-util  -v -d 0483:df11 -i 0 -a 0 -s 0x08000000 -D build/iobox-stm32/iobox-stm32.bin
	# 0483:df11 is for STM32 F4 only
	actionString  = '$DFU_STM32_PROGRAMMER -v -d 0483:df11 -i 0 -a 0 -s 0x08000000:leave -D $SOURCE'

	action = Action(actionString, cmdstr = "$DFU_STM32_PROGRAMMER_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if ARGUMENTS.get('verbose') != '1':
		env['DFU_STM32_PROGRAMMER_COMSTR']       = "dfu_stm32_programmer: program $SOURCE"
	
	# Name of the binary program to run
	env['DFU_STM32_PROGRAMMER'] = 'dfu-util'
	
	env.AddMethod(dfu_stm32_programmer_flash, 'DfuStm32Programmer')

def exists(env):
	return env.Detect('dfu_stm32_programmer')

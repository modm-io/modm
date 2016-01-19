#!/usr/bin/env python2
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All Rights Reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
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

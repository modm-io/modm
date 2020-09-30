#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Sascha Schade
# Copyright (c) 2014, 2016, Daniel Krebs
# Copyright (c) 2017, Michael Thies
# Copyright (c) 2019, Raphael Lehmann
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
# env.Alias('dfu', env.DfuStm32Programmer(env.Bin(program)))
#
# Call
#   scons dfu
# from your command line

import platform
from SCons.Script import *

# -----------------------------------------------------------------------------
def dfu_stm32_programmer_flash(env, source, alias='dfu_stm32_program'):
	actionString  = '$DFU_STM32_PROGRAMMER -v -E2 -R -i 0 -a 0 -s 0x08000000:leave -D $SOURCE'

	action = Action(actionString, cmdstr = "$DFU_STM32_PROGRAMMER_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get('verbose'):
		env['DFU_STM32_PROGRAMMER_COMSTR'] = \
			"{0}.-------------- {1}$SOURCE\n" \
			"{0}'-----DFU-----> {2}$CONFIG_DEVICE_NAME{3}" \
			.format("\033[;0;32m", "\033[;0;33m", "\033[;1;33m", "\033[;0;0m")

	# Name of the binary program to run
	env['DFU_STM32_PROGRAMMER'] = 'dfu-util'

	env.AddMethod(dfu_stm32_programmer_flash, 'ProgramDFU')

def exists(env):
	return env.Detect('dfu_stm32_programmer')

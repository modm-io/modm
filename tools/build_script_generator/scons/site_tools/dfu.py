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
def program_dfu(env, source):
	delay = ARGUMENTS.get("delay", "5")
	serial = ARGUMENTS.get("serial")
	serial = " -S {}".format(serial) if serial else ""
	flash_address = env.get("CONFIG_FLASH_ADDRESS", 0x08000000)
	actionString  = 'dfu-util -v -E{}{} -R -i 0 -a 0 -s {}:leave -D $SOURCE'.format(delay, serial, flash_address)
	return env.AlwaysBuildAction(actionString, "$PROGRAM_DFU_COMSTR", source)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(program_dfu, 'ProgramDFU')

def exists(env):
	return env.Detect('dfu-util')

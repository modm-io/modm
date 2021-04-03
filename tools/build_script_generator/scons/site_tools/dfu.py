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
def program_dfu(env, source, alias='program_dfu'):
	delay = ARGUMENTS.get("delay", "5")
	actionString  = 'dfu-util -v -E{} -R -i 0 -a 0 -s 0x08000000:leave -D $SOURCE'.format(delay)

	action = Action(actionString, cmdstr="$PROGRAM_DFU_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(program_dfu, 'ProgramDFU')

def exists(env):
	return env.Detect('dfu-util')

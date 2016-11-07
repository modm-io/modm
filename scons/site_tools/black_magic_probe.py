#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, Sascha Schade
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import platform
from SCons.Script import *

# -----------------------------------------------------------------------------
def black_magic_probe_run(env, source, alias='black_magic_probe_run'):
	action = Action('$ARM_NONE_EABI_GDB '
		'-ex "target extended-remote $BLACK_MAGIC_PROBE_PORT" '
		'-ex "monitor swdp_scan" '
		'-ex "attach 1" '
		'-ex "load $SOURCE" '
		'-ex "detach" '
		'-ex "quit" ',
		cmdstr="$BLACK_MAGIC_PROBE_COMSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get('verbose'):
		env['BLACK_MAGIC_PROBE_COMSTR'] = "Black Magic Probe: program $SOURCE"

	env['ARM_NONE_EABI_GDB'] = 'arm-none-eabi-gdb'

	env.AddMethod(black_magic_probe_run, 'BlackMagicProbe')

def exists(env):
	return True

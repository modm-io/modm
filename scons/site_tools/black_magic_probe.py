#!/usr/bin/env python2
# Copyright (c) 2016, Roboterclub Aachen e. V.
# All Rights Reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
#
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

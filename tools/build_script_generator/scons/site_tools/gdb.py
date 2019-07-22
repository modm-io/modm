#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
from SCons.Script import *
import subprocess
import signal

def run_openocd_gdb(target, source, env):
	# We have to start openocd in its own session ID, so that Ctrl-C in GDB
	# does not kill OpenOCD. See https://github.com/RIOT-OS/RIOT/pull/3619.
	config = env.Listify(env.get("MODM_OPENOCD_CONFIGFILES", []))
	config = " ".join(map("-f \"{}\"".format, map(env.subst, config)))
	openocd = subprocess.Popen("openocd {} -c \"log_output /dev/null\"".format(config),
	                           cwd=os.getcwd(), shell=True, preexec_fn=os.setsid)
	# This call is blocking
	config =  env.Listify(env.get("MODM_GDBINIT", []))
	config += env.Listify(env.get("MODM_OPENOCD_GDBINIT", []))
	config = " ".join(map("-x \"{}\"".format, map(env.subst, config)))
	subprocess.call("arm-none-eabi-gdb {} {}".format(config, source[0]),
	                cwd=os.getcwd(), shell=True)
	# Then kill just the background OpenOCD process
	os.killpg(os.getpgid(openocd.pid), signal.SIGTERM)
	return 0

def gdb_arm_none_eabi_debug(env, source, alias="gdb_arm_none_eabi_debug"):
	action = Action(run_openocd_gdb, cmdstr="$OPENOCDGDBSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get("verbose"):
		env["OPENOCDGDBSTR"] = "%s.------GDB----> %s$SOURCE\n" \
	                           "%s'----OpenOCD--> %s$CONFIG_DEVICE_NAME%s" % \
	                           ("\033[;0;32m", "\033[;0;33m", "\033[;0;32m", "\033[;1;33m", "\033[;0;0m")

	env.AddMethod(gdb_arm_none_eabi_debug, "OpenOcdGdb")

def exists(env):
	return env.Detect("openocd") and env.Detect("arm-none-eabi-gdb")

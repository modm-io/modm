#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2011-2012, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2016-2017, 2023, Niklas Hauser
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2016, Tarik TIRE
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import platform
from SCons.Script import *

from modm_tools import gdb, openocd

# -----------------------------------------------------------------------------
def _config(env):
	oconfig = env.SubstList("MODM_OPENOCD_CONFIGFILES")
	osearch = env.SubstList("MODM_OPENOCD_SEARCHDIRS")
	gconfig  = env.SubstList("MODM_GDBINIT_OPENOCD", "MODM_GDBINIT")
	gcmds = env.SubstList("MODM_GDB_COMMANDS")
	return (oconfig, osearch, gconfig, gcmds)

# -----------------------------------------------------------------------------
def debug_openocd(env, source):
	def call_debug_openocd(target, source, env):
		oconfig, osearch, gconfig, gcmds = _config(env)
		backend = openocd.OpenOcdBackend(config=oconfig, search=osearch)
		gdb.call(backend, source=source[0], config=gconfig,
				 commands=gcmds, ui=ARGUMENTS.get("ui", "tui"))
	return env.AlwaysBuildAction(call_debug_openocd, "$DEBUG_OPENOCD_COMSTR", source)

def coredump_openocd(env):
	def call_coredump_openocd(target, source, env):
		oconfig, osearch, gconfig, gcmds = _config(env)
		backend = openocd.OpenOcdBackend(config=oconfig, search=osearch)
		gcmds += ["modm_coredump", "modm_build_id", "quit"]
		gdb.call(backend, config=gconfig, commands=gcmds)
	return env.AlwaysBuildAction(call_coredump_openocd, "$COREDUMP_OPENOCD_COMSTR")

# -----------------------------------------------------------------------------
def program_openocd(env, source):
	def call_program_openocd(target, source, env):
		oconfig, osearch, *_ = _config(env)
		openocd.program(source=str(source[0]), config=oconfig, search=osearch)
	return env.AlwaysBuildAction(call_program_openocd, "$PROGRAM_OPENOCD_COMSTR", source)

def reset_openocd(env):
	def call_reset_openocd(target, source, env):
		oconfig, osearch, *_ = _config(env)
		openocd.reset(config=oconfig, search=osearch)
	return env.AlwaysBuildAction(call_reset_openocd, "$RESET_OPENOCD_COMSTR")

def run_openocd(env):
	def call_run_openocd(target, source, env):
		oconfig, osearch, *_ = _config(env)
		openocd.call(config=oconfig, search=osearch, verbose=True)
	return env.AlwaysBuildAction(call_run_openocd, "$RUN_OPENOCD_COMSTR")

# -----------------------------------------------------------------------------
def log_openocd_itm(env):
	def run_openocd_itm(target, source, env):
		fcpu = ARGUMENTS.get("fcpu", None)
		if fcpu is None:
			print("\n    You must provide the current CPU/HCLK frequency as `fcpu={Hz}` argument!\n")
			return -1
		baudrate = ARGUMENTS.get("baudrate", None)
		oconfig, osearch, *_ = _config(env)
		backend = openocd.OpenOcdBackend(config=oconfig, search=osearch)
		openocd.itm(backend, fcpu, baudrate)
	return env.AlwaysBuildAction(run_openocd_itm, "$ITM_OPENOCD_COMSTR")

def log_openocd_rtt(env):
	def run_openocd_rtt(target, source, env):
		oconfig, osearch, *_ = _config(env)
		backend = openocd.OpenOcdBackend(config=oconfig, search=osearch)
		openocd.rtt(backend, int(ARGUMENTS.get("channel", 0)))
	return env.AlwaysBuildAction(run_openocd_rtt, "$RTT_OPENOCD_COMSTR")

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(program_openocd, "ProgramOpenOcd")
	env.AddMethod(debug_openocd, "DebugOpenOcd")
	env.AddMethod(coredump_openocd, "CoredumpOpenOcd")
	env.AddMethod(reset_openocd, "ResetOpenOcd")
	env.AddMethod(run_openocd, "OpenOcd")

	env.AddMethod(log_openocd_itm, "LogItmOpenOcd")
	env.AddMethod(log_openocd_rtt, "LogRttOpenOcd")

def exists(env):
	return env.Detect("openocd")

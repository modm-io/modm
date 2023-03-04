#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2023, Niklas Hauser
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

from modm_tools import gdb, jlink
from modm_tools.jlink import JLinkBackend

# -----------------------------------------------------------------------------
def _config(env):
	device = env.subst(env["MODM_JLINK_DEVICE"])
	gconfig  = env.SubstList("MODM_GDBINIT_JLINK", "MODM_GDBINIT")
	gcmds = env.SubstList("MODM_GDB_COMMANDS")
	return (device, gconfig, gcmds)

# -----------------------------------------------------------------------------
def debug_jlink(env, source):
	def call_debug_jlink(target, source, env):
		device, gconfig, gcmds = _config(env)
		backend = JLinkBackend(device)
		gdb.call(backend, source=source[0], config=gconfig,
				 commands=gcmds, ui=ARGUMENTS.get("ui", "tui"))
	return env.AlwaysBuildAction(call_debug_jlink, "$DEBUG_JLINK_COMSTR", source)

def coredump_jlink(env):
	def call_coredump_jlink(target, source, env):
		device, gconfig, gcmds = _config(env)
		backend = JLinkBackend(device)
		gcmds += ["modm_coredump", "modm_build_id", "quit"]
		gdb.call(backend, config=gconfig, commands=gcmds)
	return env.AlwaysBuildAction(call_coredump_jlink, "$COREDUMP_JLINK_COMSTR")

# -----------------------------------------------------------------------------
def program_jlink(env, source):
	def call_program_jlink(target, source, env):
		jlink.program(_config(env)[0], str(source[0]))
	return env.AlwaysBuildAction(call_program_jlink, "$PROGRAM_JLINK_COMSTR", source)

def reset_jlink(env):
	def call_reset_jlink(target, source, env):
		jlink.reset(_config(env)[0])
	return env.AlwaysBuildAction(call_reset_jlink, "$RESET_JLINK_COMSTR")

def run_jlink(env):
	def call_run_jlink(target, source, env):
		jlink.call(_config(env)[0], verbose=True)
	return env.AlwaysBuildAction(call_run_jlink, "$RUN_JLINK_COMSTR")

# -----------------------------------------------------------------------------
def log_jlink_itm(env):
	def run_jlink_itm(target, source, env):
		baudrate = ARGUMENTS.get("baudrate", None)
		jlink.itm(_config(env)[0], baudrate)
	return env.AlwaysBuildAction(run_jlink_itm, "$ITM_JLINK_COMSTR")

def log_jlink_rtt(env):
	def run_jlink_rtt(target, source, env):
		backend = jlink.JLinkBackend(_config(env)[0])
		jlink.rtt(backend, int(ARGUMENTS.get("channel", 0)))
	return env.AlwaysBuildAction(run_jlink_rtt, "$RTT_JLINK_COMSTR")

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(program_jlink, "ProgramJLink")
	env.AddMethod(debug_jlink, "DebugJLink")
	env.AddMethod(coredump_jlink, "CoredumpJLink")
	env.AddMethod(reset_jlink, "ResetJLink")
	env.AddMethod(run_jlink, "JLink")

	env.AddMethod(log_jlink_itm, "LogItmJLink")
	env.AddMethod(log_jlink_rtt, "LogRttJLink")

def exists(env):
	return env.Detect("jlink")

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
from modm_tools import openocd_gdb

def execute_action(env, source, mode):
    # Build tool environment
    config_openocd = env.Listify(env.get("MODM_OPENOCD_CONFIGFILES", []))
    config_gdb  = env.Listify(env.get("MODM_OPENOCD_GDBINIT", []))
    config_gdb += env.Listify(env.get("MODM_GDBINIT", []))
    tool_env = {
        "CONFIG_OPENOCD": map(env.subst, config_openocd),
        "CONFIG_GDB": map(env.subst, config_gdb),
        "CONFIG_SOURCES": [source[0]],
    }
    return openocd_gdb.execute(tool_env, mode)

# -----------------------------------------------------------------------------
def execute_gdbtui(target, source, env):
    return execute_action(env, source, mode="tui")

def execute_gdbgui(target, source, env):
    return execute_action(env, source, mode="gui")

# -----------------------------------------------------------------------------
def build_gdbtui(env, source, alias="build_gdbtui"):
    action = Action(execute_gdbtui, cmdstr="$OPENOCDGDBSTR")
    return env.AlwaysBuild(env.Alias(alias, source, action))

def build_gdbgui(env, source, alias="build_gdbgui"):
    action = Action(execute_gdbgui, cmdstr="$OPENOCDGDBSTR")
    return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
    # build messages
    if not ARGUMENTS.get("verbose"):
        env["OPENOCDGDBSTR"] = \
            "%s.------GDB----> %s$SOURCE\n" \
            "%s'----OpenOCD--> %s$CONFIG_DEVICE_NAME%s" % \
            ("\033[;0;32m", "\033[;0;33m", "\033[;0;32m", "\033[;1;33m", "\033[;0;0m")

    env.AddMethod(build_gdbtui, "OpenOcdGdbTui")
    env.AddMethod(build_gdbgui, "OpenOcdGdbGui")

def exists(env):
    return env.Detect("openocd") and env.Detect("arm-none-eabi-gdb")

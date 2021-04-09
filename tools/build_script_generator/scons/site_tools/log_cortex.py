#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2019, Niklas Hauser
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
import tempfile
from modm_tools.openocd import OpenOcdBackend, log_itm, log_rtt

# -----------------------------------------------------------------------------
def log_openocd_itm(env, alias="log_openocd_itm"):
    def run_openocd_itm(target, source, env):
        fcpu = ARGUMENTS.get("fcpu", None)
        if fcpu is None:
            print("\n    You must provide the current CPU/HCLK frequency as `fcpu={Hz}` argument!\n")
            return -1
        baudrate = ARGUMENTS.get("baudrate", None)

        search = map(env.subst, env.Listify(env.get("MODM_OPENOCD_SEARCHDIRS", [])))
        config = map(env.subst, env.Listify(env.get("MODM_OPENOCD_CONFIGFILES", [])))
        backend = OpenOcdBackend(config=config, search=search)

        log_itm(backend, fcpu, baudrate)
        return 0

    action = Action(run_openocd_itm, cmdstr="$ITM_OPENOCD_COMSTR")
    return env.AlwaysBuild(env.Alias(alias, None, action))

# -----------------------------------------------------------------------------
def log_openocd_rtt(env, alias="log_openocd_rtt"):
    def run_openocd_rtt(target, source, env):
        search = map(env.subst, env.Listify(env.get("MODM_OPENOCD_SEARCHDIRS", [])))
        config = map(env.subst, env.Listify(env.get("MODM_OPENOCD_CONFIGFILES", [])))
        backend = OpenOcdBackend(config=config, search=search)

        log_rtt(backend, int(ARGUMENTS.get("channel", 0)))
        return 0

    action = Action(run_openocd_rtt, cmdstr="$RTT_OPENOCD_COMSTR")
    return env.AlwaysBuild(env.Alias(alias, None, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
    env.AddMethod(log_openocd_itm, "LogItmOpenOcd")
    env.AddMethod(log_openocd_rtt, "LogRttOpenOcd")

def exists(env):
    return env.Detect("openocd") and env.Detect("tail")

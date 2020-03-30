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
from modm_tools.openocd import OpenOcdBackend, log_itm

# -----------------------------------------------------------------------------
def log_openocd_itm(env, alias="openocd_itm"):
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

    action = Action(run_openocd_itm, cmdstr="$OPENOCD_ITM_STR")
    return env.AlwaysBuild(env.Alias(alias, None, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
    if ARGUMENTS.get('verbose') != '1':
        env["OPENOCD_ITM_STR"] = \
            "{0}.----OpenOCD--> {1}Single Wire Viewer\n" \
            "{0}'------SWO----- {2}$CONFIG_DEVICE_NAME{3}" \
            .format("\033[;0;32m", "\033[;0;33m", "\033[;1;33m", "\033[;0;0m")

    env.AddMethod(log_openocd_itm, "LogItmOpenOcd")

def exists(env):
    return env.Detect("openocd") and env.Detect("tail")

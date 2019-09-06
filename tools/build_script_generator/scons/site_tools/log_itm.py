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
import signal

# -----------------------------------------------------------------------------
def run_openocd_itm(target, source, env):
    # Provide additional search paths via the OPENOCD_SCRIPTS environment variable
    # See http://openocd.org/doc/html/Running.html
    env["ENV"]["OPENOCD_SCRIPTS"] = os.environ.get("OPENOCD_SCRIPTS", "")
    search = env.Listify(env.get("MODM_OPENOCD_SEARCHDIRS", []))
    config = env.Listify(env.get("MODM_OPENOCD_CONFIGFILES", []))
    openocdcmd = '{} {}'.format(
            " ".join(map('-s "{}"'.format, map(env.subst, search))),
            " ".join(map('-f "{}"'.format, map(env.subst, config))))

    fcpu = ARGUMENTS.get("fcpu", None)
    if fcpu is None:
        print("\n    You must provide the current CPU/HCLK frequency as `fcpu={Hz}` argument!\n")
        return -1
    baudrate = ARGUMENTS.get("baudrate", "")

    with tempfile.NamedTemporaryFile() as tmpfile:
        itm_config = "modm_itm_log {} {} {}".format(tmpfile.name, fcpu, baudrate)
        openocdcmd += ' -c "{}"'.format(itm_config)
        openocdcmd = '-c "log_output /dev/null"' + openocdcmd

        # Start OpenOCD in the background
        openocd = subprocess.Popen('openocd ' + openocdcmd, cwd=os.getcwd(), shell=True)
        # Start a blocking call to monitor the log file
        subprocess.call("tail -f {}".format(tmpfile.name), cwd=os.getcwd(), shell=True)
        # kill OpenOCD process when Ctrl-C happened
        os.killpg(os.getpgid(openocd.pid), signal.SIGTERM)

    return 0

def openocd_itm(env, alias="openocd_itm"):
    action = Action(run_openocd_itm, cmdstr="$OPENOCDITMSTR")
    return env.AlwaysBuild(env.Alias(alias, None, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
    env["OPENOCD"] = "openocd"
    if ARGUMENTS.get('verbose') != '1':
        env["OPENOCDITMSTR"] = ("{0}.----OpenOCD--> {1}Single Wire Viewer\n"
                                "{0}'------SWO----- {2}$CONFIG_DEVICE_NAME{3}"
                                .format("\033[;0;32m", "\033[;0;33m", "\033[;1;33m", "\033[;0;0m"))

    env.AddMethod(openocd_itm, "OpenOcdItm")

def exists(env):
    return env.Detect("openocd") and env.Detect("tail")

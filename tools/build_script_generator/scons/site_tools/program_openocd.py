#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2011-2012, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2016-2017, Niklas Hauser
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

# -----------------------------------------------------------------------------
def openocd_run(env, source, commands=[], alias="openocd_run"):
	# Provide additional search paths via the OPENOCD_SCRIPTS environment variable
	# See http://openocd.org/doc/html/Running.html
	env["ENV"]["OPENOCD_SCRIPTS"] = os.environ.get("OPENOCD_SCRIPTS", "")

	search = env.Listify(env.get("MODM_OPENOCD_SEARCHDIRS", []))
	config = env.Listify(env.get("MODM_OPENOCD_CONFIGFILES", []))

	openocdcmd = "$OPENOCD {} {} {}".format(
		" ".join(map("-s \"{}\"".format, search)),
		" ".join(map("-f \"{}\"".format, config)),
		" ".join(map("-c \"{}\"".format, env.Listify(commands)))
	)
	action = Action(openocdcmd, cmdstr="$OPENOCDSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env["OPENOCD"] = "openocd"
	if ARGUMENTS.get('verbose') != '1':
		env["OPENOCDSTR"] = "%s.----OpenOCD--- %s$SOURCE\n" \
	                        "%s'-------------> %s$CONFIG_DEVICE_NAME%s" % \
	                        ("\033[;0;32m", "\033[;0;33m", "\033[;0;32m", "\033[;1;33m", "\033[;0;0m")

	env.AddMethod(openocd_run, "OpenOcd")

def exists(env):
	return env.Detect("openocd")

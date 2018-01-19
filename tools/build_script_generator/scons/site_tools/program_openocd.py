#!/usr/bin/env python2
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
def openocd_run(env, source, alias="openocd_run"):
	# Provide additional search paths via the OPENOCD_SCRIPTS environment variable
	# See http://openocd.org/doc/html/Running.html
	env["ENV"]["OPENOCD_SCRIPTS"] = os.environ.get("OPENOCD_SCRIPTS", "")

	openocdcmd = "$OPENOCD {} {} {}".format(
		" ".join(map("-s \"{}\"".format, env.get("CONFIG_OPENOCD_SEARCHDIRS", []))),
		" ".join(map("-f \"{}\"".format, env.get("CONFIG_OPENOCD_CONFIGFILES", []))),
		" ".join(map("-c \"{}\"".format, env.get("CONFIG_OPENOCD_COMMANDS", [])))
	)
	action = Action(openocdcmd, cmdstr="$INSTALLSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env["OPENOCD"] = "openocd"
	env.AddMethod(openocd_run, "OpenOcd")

def exists(env):
	return env.Detect("openocd")

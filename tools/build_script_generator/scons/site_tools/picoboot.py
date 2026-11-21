#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2026, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

from SCons.Script import *

# -----------------------------------------------------------------------------
def program_picoboot(env, source):
	serial = ARGUMENTS.get("serial")
	action = "picotool load --verify --execute $SOURCE"
	if serial: action += " --ser {}".format(serial)
	return env.AlwaysBuildAction(action, "$PROGRAM_PICOBOOT_COMSTR", source)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(program_picoboot, "ProgramPicoboot")

def exists(env):
	return env.Detect("picotool")

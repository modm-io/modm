#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Niklas Hauser
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2017, Michael Thies
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

from SCons.Script import *

import os
import re
import string
import datetime

from modm_tools import unit_test

# -----------------------------------------------------------------------------
def unittest_action(target, source, env):
	unit_test.render_runner(headers=(str(s) for s in source),
	                       destination=target[0].abspath)
	return 0

def unittest_emitter(target, source, env):
	headers = []
	for file in source:
		if file.name.endswith('_test.hpp'):
			headers.append(file)
	return target, headers

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env['BUILDERS']['UnittestRunner'] = env.Builder(
		action = Action(unittest_action, cmdstr="$UNITTESTCOMSTR"),
		suffix = '.cpp',
		emitter = unittest_emitter,
		target_factory = env.fs.File)

def exists(env):
	return True

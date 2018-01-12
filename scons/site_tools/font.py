#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009, Martin Rosekeit
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Niklas Hauser
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import os.path

script_path = os.path.dirname(__file__)
command = os.path.join(script_path, "../../tools/font_creator/font_export.py")

# -----------------------------------------------------------------------------
def font_action(target, source, env):
	infile  = str(source[0])
	outfile = os.path.splitext(str(target[0]))[0]

	os.system('python3 "%s" "%s" "%s"' % (command, infile, outfile))

def font_string(target, source, env):
	return "Create Font: '%s'" % (str(source[0]))

def font_emitter(target, source, env):
	header = os.path.splitext(str(target[0]))[0] + ".hpp"
	target.append(header)
	env.Depends(target, command)
	return (target, source)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.Append(
		BUILDERS = {
			'Font': env.Builder(
				action = env.Action(font_action, font_string),
				suffix = '.cpp',
				src_suffix = '.font',
				emitter = font_emitter,
				single_source = True),
	})

def exists(env):
	return True

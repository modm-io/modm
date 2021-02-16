#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2014, 2017, 2020, Niklas Hauser
# Copyright (c) 2013, Kevin Läufer
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
from os.path import splitext, dirname
from modm_tools import bitmap

# -----------------------------------------------------------------------------
def bitmap_action(target, source, env):
	bitmap.convert(image=str(source[0]),
	               outpath=dirname(str(target[0])))

def bitmap_emitter(target, source, env):
	header = splitext(str(target[0]))[0] + ".hpp"
	target.append(header)
	return (target, source)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env['BUILDERS']['Bitmap'] = env.Builder(
		action = Action(bitmap_action, cmdstr="$BITMAPCOMSTR"),
		suffix = '.cpp',
		src_suffix = '.pbm',
		emitter = bitmap_emitter,
		single_source = True)

def exists(env):
	return True

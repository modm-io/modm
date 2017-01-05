#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2014, Niklas Hauser
# Copyright (c) 2013, Kevin Laeufer
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import string
import os
import re
import math
from SCons.Script import *

# -----------------------------------------------------------------------------
template_source = """
#include <modm/architecture/driver/accessor.hpp>

namespace bitmap
{
	FLASH_STORAGE(uint8_t ${name}[]) =
	{
		${width}, ${height},
		${array}
	};
}

"""

template_header = """
#ifndef ${include_guard}
#define ${include_guard}

#include <modm/architecture/driver/accessor.hpp>

namespace bitmap
{
	/**
	 * \\brief	Generated bitmap
	 *
	 * Generated from file "${filename}".
	 *
	 * - Width  : ${width}
	 * - Height : ${height}
	 *
	 * \\ingroup	image
	 */
	EXTERN_FLASH_STORAGE(uint8_t ${name}[]);
}

#endif // ${include_guard}

"""

# -----------------------------------------------------------------------------
def bitmap_action(target, source, env):
	filename = str(source[0])

	input = open(filename).read()
	if input[0:3] != "P1\n":
		env.Error("Error: format needs to be a portable bitmap in ascii format (file descriptor 'P1')!")
		exit(1)

	input = input[3:]

	while input[0] == "#" or input[0] in string.whitespace:
		# switch to the next line
		input = input[input.find("\n") + 1:]

	result = re.match("^(\d+) (\d+)\n", input)
	if not result:
		env.Error("Error: bad format!")

	width = int(result.group(1))
	height = int(result.group(2))

	rows = int(math.ceil(height / 8.0))

	# now we finally have the raw data
	input = input[result.end():]
	input = input.replace("\n", "")

	data = []
	for y in range(rows):
		data.append([0 for row in range(width)])

	for y in range(height):
		for x in range(width):
			index = x + y * width
			if input[index] == "1":
				data[y / 8][x] |= 1 << (y % 8)

	array = []
	for y in range(rows):
		line = []
		for x in range(width):
			line.append("0x%02x," % data[y][x])
		array.append(" ".join(line))

	basename = os.path.splitext(os.path.basename(str(target[0])))[0]
	substitutions = {
		'name': basename,
		'filename': str(source[0]),
		'width': width,
		'height': height,
		'array': "\n\t\t".join(array),
		'include_guard': "BITMAP__" + basename.upper().replace(" ", "_") + "_HPP"
	}

	output = string.Template(template_source).safe_substitute(substitutions)
	open(target[0].path, 'w').write(output)

	output = string.Template(template_header).safe_substitute(substitutions)
	open(target[1].path, 'w').write(output)

def bitmap_string(target, source, env):
	return "Create Bitmap: '%s'" % (str(target[0]))

def bitmap_emitter(target, source, env):
	header = os.path.splitext(str(target[0]))[0] + ".hpp"
	target.append(header)
	return (target, source)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.Append(
		BUILDERS = {
			'Bitmap': env.Builder(
				action = env.Action(bitmap_action, bitmap_string),
				suffix = '.cpp',
				src_suffix = '.pbm',
				emitter = bitmap_emitter,
				single_source = True),
	})

def exists(env):
	return True

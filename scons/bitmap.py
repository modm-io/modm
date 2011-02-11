#!/usr/bin/env python
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Roboterclub Aachen e.V. nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# $Id$

import string
import os
import re
import math
from SCons.Script import *

# -----------------------------------------------------------------------------
template_source = """
#include <xpcc/architecture/driver/accessor.hpp>

namespace bitmap
{
	FLASH(uint8_t ${name}[]) =
	{
		${width}, ${height},
		${array}
	};
}

"""

template_header = """
#ifndef ${include_guard}
#define ${include_guard}

#include <xpcc/architecture/driver/accessor.hpp>

namespace bitmap
{
	/**
	 * \\brief	Generated bitmap
	 * 
	 * Generated from file "${filename}".
	 * 
	 * - Width  : ${width}
	 * - Height : ${height}
	 */
	EXTERN_FLASH(uint8_t ${name}[]);
}

#endif // ${include_guard}

"""

# -----------------------------------------------------------------------------
def bitmap_action(target, source, env):
	filename = str(source[0])
	
	input = open(filename).read()
	if input[0:3] != "P1\n":
		print "Error: format needs to be a portable bitmap in ascii format (file descriptor 'P1')!"
		exit(1)
	
	input = input[3:]
	
	while input[0] == "#" or input[0] in string.whitespace:
		# switch to the next line
		input = input[input.find("\n") + 1:]
	
	result = re.match("^(\d+) (\d+)\n", input)
	if not result:
		print "Error: bad format!"
	
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

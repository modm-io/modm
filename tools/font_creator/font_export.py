#!/usr/bin/env python

import os
import string
import re
import math
import datetime

# -----------------------------------------------------------------------------
template_copyright = """\
// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) %s, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------\
""" % datetime.datetime.today().year

# -----------------------------------------------------------------------------
template_source = """\
${copyright}
// created with FontCreator 3.0

#include <xpcc/architecture/driver/accessor.hpp>

namespace xpcc
{
	namespace font
	{
		FLASH_STORAGE(uint8_t ${array_name}[]) =
		{
			${size_low}, ${size_high}, // total size of this array
			${width},	// width (may vary)
			${height},	// height
			${hspace},	// hspace
			${vspace}, 	// vspace
			${first},	// first char
			${count},	// char count
			
			// char widths
			// for each character the separate width in pixels
			${char_width}
			
			// font data
			// bit field of all characters
			${font_data}
		};
	}
}

"""

# -----------------------------------------------------------------------------
template_header = """\
${copyright}

#ifndef ${include_guard}
#define	${include_guard}

#include <xpcc/architecture/driver/accessor.hpp>

namespace xpcc
{
	namespace font
	{
		/**
		 * \\brief	${font_name}
		 * 
		 * - ${width_string} : ${width}
		 * - height          : ${height}
		 * - hspace          : ${hspace}
		 * - vspace          : ${vspace}
		 * - first char      : ${first}
		 * - last char       : ${last}
		 * - number of chars : ${count}
		 * - size in bytes   : ${size}
		 * 
		 * \\ingroup	font
		 */
		EXTERN_FLASH_STORAGE(uint8_t ${array_name}[]);
	}
}

#endif	// ${include_guard}

"""

# -----------------------------------------------------------------------------
class ParseException(Exception):
	def __init__(self, msg, line):
		Exception.__init__(self, msg)
		self.line = line

class Font:
	def __init__(self):
		self.name = None
		self.height = None
		self.hspace = None
		self.vspace = None
		self.first_char = None
		self.chars = []

class Char:
	def __init__(self, index, height):
		self.index = index
		self.width = None
		self.height = height
		self.data = []
		
		self.rows = int(math.ceil(height / 8.0))

# -----------------------------------------------------------------------------
def read_font_file(filename):
	char_mode = False
	char_line_count = 0
	char_line_index = 0
	char_last_number = None	# TODO
	char = None
	
	font = Font()
	
	lines = open(filename).readlines()
	for line_number, line in enumerate(lines):
		if char_mode:
			result = re.match("^\[([ #]+)\]\n", line)
			if not result:
				raise ParseException("Illegal Format in: %s" % line[:-1], line_number)
			
			width = len(result.group(1))
			if char.width is None:
				char.data = [0] * (char.rows * width)
			else:
				if width != char.width:
					raise ParseException("Illegal width for char %i" % char.number, line_number)
			char.width = width
			
			index = 0
			for c in result.group(1):
				if c == " ":
					pass
				elif c == "#":
					y = char_line_index / 8
					offset = y * char.width
					char.data[offset + index] |= 1 << (char_line_index % 8)
				else:
					raise ParseException("Illegal character in line: %s" % line, line_number)
				index += 1
			
			char_line_index += 1
			
			char_line_count -= 1
			if char_line_count == 0:
				char_mode = False
		elif line[0] == '#':
			result = re.match("^#(\w+)[ \t]+:[ \t]+(.*)\n", line)
			if not result:
				print "Error in: ", line
				exit(1)
			else:
				key = result.group(1)
				value = result.group(2)
				
				if key == "font":
					font.name = value
				elif key == "width":
					pass	# unused
				elif key == "height":
					font.height = int(value)
				elif key == "hspace":
					font.hspace = int(value)
				elif key == "vspace":
					font.vspace = int(value)
				elif key == "char":
					charMatch = re.match("^(\d+)([ \t]*.*)", value)
					if not charMatch:
						raise ParseException("Illegal Format in: %s" % line[:-1], line_number)
					number = int(charMatch.group(1))
					
					if font.first_char is None:
						font.first_char = number
					else:
						if number != (char_last_number + 1):
							raise ParseException("Unexpected character id %i" % number, line_number)
					char_last_number = number
					
					if font.height is None:
						raise ParseException("'height' not set!")
					char_mode = True
					char_line_count = font.height
					char_line_index = 0
					
					char = Char(number, font.height)
					font.chars.append(char)
				else:
					raise ParseException("Unknown key '%s'" % key, line_number)
		elif line[0] in string.whitespace:
			pass
		else:
			# TODO better error message
			raise ParseException("Illegal Format: %s" % line[:-1], line_number)
	
	return font

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	try:
		filename = os.sys.argv[1]
		if not filename.endswith('.font'):
			raise
		outfile = os.sys.argv[2]
	except:
		print "usage: %s *.font outfile" % os.sys.argv[0]
		exit(1)
	
	try:
		font = read_font_file(filename)
	except ParseException as e:
		print "Error in line %i: " % e.line, e
		exit(1)
	
	width_histogram = {}
	char_width = []
	char_width_line = ""
	char_line_count = 0
	font_data = []
	
	# 8 byte header, width table
	size = 8 + len(font.chars)
	for char in font.chars:
		size += len(char.data)
		
		char_width_line += "%2i, " % char.width
		
		char_line_count += 1
		if char_line_count >= 10:
			char_line_count = 0
			
			char_width.append(char_width_line)
			char_width_line = ""
		
		data = ""
		for c in char.data:
			data += "0x%02X, " % c
		data += "// %i" % char.index
		font_data.append(data)
		
		width_histogram[char.width] = width_histogram.get(char.width, 0) + 1
	
	if char_width_line != "":
		char_width.append(char_width_line)
	
	preferred_width = 0
	max = 0
	for key, value in width_histogram.items():
		if value > max:
			preferred_width = key
			max = value
	
	substitutions = {
		'copyright': template_copyright,
		'font_name': font.name,
		'array_name': ''.join([s[0].upper() + s[1:] for s in font.name.split(' ')]),
		'size': size,
		'size_low': "0x%02X" % (size & 0xff),
		'size_high': "0x%02X" % (size >> 8),
		'width': preferred_width,
		'width_string': "fixed width    " if (len(width_histogram) == 1) else "preferred width",
		'height': font.height,
		'hspace': font.hspace,
		'vspace': font.vspace,
		'first': font.first_char,
		'last': font.first_char + len(font.chars),
		'count': len(font.chars),
		'char_width': "\n\t\t\t".join(char_width),
		'font_data': "\n\t\t\t".join(font_data),
		'include_guard': "XPCC_FONT__" + os.path.basename(outfile).upper().replace(" ", "_") + "_HPP"
	}
	
	output = string.Template(template_source).safe_substitute(substitutions)
	open(outfile + ".cpp", 'w').write(output)
	
	output = string.Template(template_header).safe_substitute(substitutions)
	open(outfile + ".hpp", 'w').write(output)

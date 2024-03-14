#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010, Fabian Greif
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import string
import re
import math

if __name__ == '__main__':
	filename = os.sys.argv[1]

	if not filename.endswith('.pbm'):
		print("usage: %s *.pbm" % os.sys.argv[0])
		exit(1)

	input = open(filename).read()
	if input[0:3] != "P1\n":
		print("Format needs to be a portable bitmap in ascii format (file descriptor 'P1')!")
		exit(1)

	input = input[3:]

	while input[0] == "#" or input[0] in string.whitespace:
		# switch to the next line
		input = input[input.find("\n") + 1:]

	result = re.match(r"^(\d+) (\d+)\n", input)
	if not result:
		print("bad format!")

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

	output = []
	for y in range(rows):
		line = []
		for x in range(width):
			line.append("0x%02x," % data[y][x])
		output.append(" ".join(line))

	print("\n".join(output))

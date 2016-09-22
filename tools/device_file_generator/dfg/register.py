# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

import re

from logger import Logger

class Register():
	""" Register
	Represents the names and masks of the registers
	and methods for smart comparison.
	"""

	def __init__(self, name=None, fields=None, size=None, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger

		if fields == None:
			fields = []
		self.name = name
		self.fields = fields
		if size == None:
			size = 1
		self.size = size

	def addField(self, name, index):
		self.fields.append({'name': name, 'index': index})

	def maskFromRegister(self):
		mask = 0
		for field in self.fields:
			mask |= (1 << field['index'])
		return mask

	def getFieldsWithPattern(self, pattern):
		results = []
		for field in self.fields:
			match = re.search(pattern, field['name'])
			if match:
				results.append(field)

		if len(results) > 0:
			return results
		return None

	def isEmpty(self):
		return len(self.fields) == 0

	def __eq__(self, other):
		if isinstance(other, Register):
			if self.name != other.name:
				return False
			if len(self.fields) != len(other.fields):
				return False

			return 	all(item in  self.fields for item in other.fields) and \
					all(item in other.fields for item in  self.fields)

		return NotImplemented

	def __ne__(self, other):
		result = self.__eq__(other)
		if result is NotImplemented:
			return result
		return not result

	def __hash__(self):
		return hash(self.name + str(self.fields))

	def __repr__(self):
		return "Register(" + str(self.name) + ")"

	def __str__(self):
		s = "\n Register: " + str(self.name)
		bW = 15
		self.fields.sort(key=lambda k : k['index'], reverse=True)
		for ii in range(self.size):
			s += "\n+" + ("-"*(bW - 1) + "+") * 8
			values = "\n|" + (" "*(bW - 1) + "|") * 8

			for field in self.fields:
				index = field['index']
				if ii * 8 <= index < (ii + 1) * 8:
					index -= ii * 8
					values = values[:2 + bW * (7 - index)] + field['name'].center(bW - 1) + values[1 + bW * ((8 - index)):]

			s = s + values + "\n+" + ("-"*(bW - 1) + "+") * 8
		return s

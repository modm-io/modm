# -*- coding: utf-8 -*-
# 
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of the Roboterclub Aachen e.V. nor the
#    names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
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
# -----------------------------------------------------------------------------

import os, sys, re
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
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
			s += "\n+" + ("-"*(bW-1) + "+")*8
			values = "\n|" + (" "*(bW-1) + "|")*8
			
			for field in self.fields:
				index = field['index']
				if ii*8 <= index < (ii+1)*8:
					index -= ii*8
					values = values[:2+bW*index] + field['name'].center(bW-1) + values[1+bW*(index+1):]
			
			s = s + values + "\n+" + ("-"*(bW-1) + "+")*8
		return s

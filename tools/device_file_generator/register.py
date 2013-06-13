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

import os, sys
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

class Register():
	""" Register
	Represents the names and masks of the registers
	and methods for smart comparison.
	"""

	def __init__(self, name=None, fields=None, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		if fields == None:
			fields = []
		self.name = name
		self.fields = fields

	def addField(self, name, mask):
		self.fields.append({'name': name, 'mask': mask})
	
	def maskFromRegister(self):
		mask = 0
		for field in self.fields:
			mask |= field['mask']
		return mask

	def isEmpty(self):
		return (len(self.fields) == 0 and self.name == None)

	def __eq__(self, other):
		if isinstance(other, DevicePeripheral):
			if self.name != other.name:
				return False
			if set(self.fields) != set(other.fields):
				return False
			return all(self.fields[key] == other.fields[key] for key in self.fields)
			
		return NotImplemented
	
	def __hash__(self):
		return hash(self.name + str(self.fields))
	
	def __ne__(self, other):
		result = self.__eq__(o)
		if result is NotImplemented:
			return result
		return not result

	def __repr__(self):
		return "Register(" + self.name + ")"

	def __str__(self):
		s = "\n Register(\n\t{'name': '" + self.name + "',\n"
		s += "\t'fields': [\n"
		for field in self.fields:
			s += "\t\t" + str(field) + "\n"
		return s + "})"

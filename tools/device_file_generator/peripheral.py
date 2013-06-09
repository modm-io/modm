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

class DevicePeripheral():
	""" DevicePeripheral
	Represents the names and masks of the peripherals control and data registers
	and methods for smart comparison.
	"""

	def __init__(self, name="", registers=[], logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		self.properties = {'name': name, 'registers': registers}

	def addRegister(self, name, fields):
		pass

	def maskFromRegister(self, register):
		mask = 0
		for field in register['fields']:
			mask |= field['mask']
		return mask

	def getComparisonDict(self, other):
		assert isinstance(other, DevicePeripheral)
		
		self_keys = set(self.properties.keys())
		other_keys = set(other.properties.keys())
		intersect = self_keys.intersection(other_keys)

		changed = set()
		unchanged = set()
		for o in intersect:
			if isinstance(other.properties[o], list):
				same = all(item in self.properties[o] for item in other.properties[o]) and \
						all(item in other.properties[o] for item in self.properties[o])
				if same == False:
					changed.add(o)
			else:
				if other.properties[o] != self.properties[o]:
					changed.add(o)
		
		unchanged = intersect - changed
		self_only = self_keys - intersect
		other_only = other_keys - intersect
		return {'different': changed, 'equal': unchanged, 'self-only': self_only, 'other-only': other_only}

	@property
	def registers(self):
		return self.properties['registers']

	@registers.setter
	def registers(self, value):
		self.properties['registers'] = value

	@property
	def name(self):
		return self.properties['name']

	@name.setter
	def name(self, value):
		self.properties['name'] = name

	def __eq__(self, other):
		if isinstance(other, DevicePeripheral):
			if self.name != other.name:
				return False
			comp = self.getComparisonDict(other)
			if len(comp['different']) == 0:
				return True
			return False
	
		return NotImplemented
	
	def __hash__(self):
		return hash(self.name + str(self.registers))
	
	def __ne__(self, other):
		result = self.__eq__(o)
		if result is NotImplemented:
			return result
		return not result

	def __repr__(self):
		return self.__str__()

	def __str__(self):
		return "DevicePeripheral(" + self.name + ")"

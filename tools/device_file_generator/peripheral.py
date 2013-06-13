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

from register import Register

class Peripheral():
	""" Peripheral
	Represents the names and masks of the peripherals control and data registers
	and methods for smart comparison.
	"""

	def __init__(self, name=None, registers=None, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		
		self.name = name
		if registers == None:
			registers = []
		self.registers = registers

	def addRegister(self, register):
		self.registers.append(register)

	def getComparisonDict(self, other):
		pass

	def getComparisonPeripheral(self, other):
		assert isinstance(other, Peripheral)
		
		dict = {'common_keys': [], 'different_keys': []}
		
		common = Peripheral(self.name)
		self_delta = Peripheral(self.name)
		other_delta = Peripheral(self.name)
		
		# compare registers
		self_regs = list(self.registers)
		other_regs = list(other.registers)
		
		common.registers = list(set(self_regs).intersection(other_regs))
		self_delta.registers = list(set(self_regs).difference(other_regs))
		other_delta.registers = list(set(other_regs).difference(self_regs))
		
		dict['common'] = common
		dict['self_delta'] = self_delta
		dict['other_delta'] = other_delta 
		
		return dict

	def isEmpty(self):
		return len(self.registers) == 0

	def __eq__(self, other):
		if isinstance(other, Peripheral):
			if self.name != other.name:
				return False
			return True
		return NotImplemented
	
	def __hash__(self):
		return hash(self.name + str(self.registers))
	
	def __ne__(self, other):
		result = self.__eq__(o)
		if result is NotImplemented:
			return result
		return not result

	def __repr__(self):
		return "Peripheral(" + str(self.name) + ")"

	def __str__(self):
		s = "\n Peripheral(\n\t{'name': '" + str(self.name) + "',\n"
		s += "\t'registers': ["
		st = ""
		for reg in self.registers:
			st += str(reg)
		st = st.replace("\n", "\n\t")
		return s + st + "]})"

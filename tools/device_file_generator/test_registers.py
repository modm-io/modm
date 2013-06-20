#!/usr/bin/env python
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
from device import Device
from avr_reader import AVRDeviceReader
from merger import DeviceMerger
import glob
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

if __name__ == "__main__":
	"""
	Some test code
	"""
	level = 'info'
	logger = Logger(level)
	devices = []
	peri_name = "EXTERNAL_INTERRUPT"
	
	for arg in sys.argv[1:]:
		if arg in ['error', 'warn', 'info', 'debug', 'disabled']:
			level = arg
			logger.setLogLevel(level)
			continue
		
		if "ATtiny" in arg or "ATmega" in arg:
			xml_path = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'AVR_devices', (arg + '*'))
			files = glob.glob(xml_path)
			for file in files:
				# deal with this here, rather than rewrite half the name merging
				if os.path.basename(file) != "ATtiny28.xml":
					part = AVRDeviceReader(file, logger)
					devices.append(Device(part, logger))
			continue
		
		peri_name = arg
	
	merger = DeviceMerger(devices, logger)
	merger.mergedByType()
	
	peripherals = []
	for dev in merger.mergedDevices:
		attributes = dev.getAttributes('peripherals')
		for attribute in attributes:
			for peripheral in attribute['value']:
				if peripheral.name == peri_name:
					peripherals.append({'ids': [attribute['id']], 'peripheral': peripheral})
	
	registers = []
	for peri in peripherals:
		for reg in peri['peripheral'].registers:
			registers.append({'ids': peri['ids'], 'register': reg})
	
	registers.sort(key=lambda k : k['register'].name)
	merged = []
	
	while len(registers) > 0:
		current = registers[0]
		registers.remove(current)
		
		matches = []
		
		for peri in registers:
			if current['register'] == peri['register']:
				matches.append(peri)
		
		for match in matches:
			registers.remove(match)
			current['ids'].extend(match['ids'])
		
		if len(matches) == 0:
			logger.warn("No match for register: " + current['register'].name + " of " + current['ids'][0].string)
		
		merged.append(current)
	
	for dev in merged:
		s = "Devices:\n"
		ii = 0
		for id in dev['ids']:
			s += id.string + " \t"
			ii += 1
			if ii > 7:
				ii = 0
				s += "\n"
		logger.info(s)
		logger.warn(str(dev['register']) + "\n")
		


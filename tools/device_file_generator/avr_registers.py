# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

import os, sys, re
from device import Device
from avr_reader import AVRDeviceReader
import glob
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

if __name__ == "__main__":
	"""
	Some test code
	"""
	level = 'warn'
	logger = Logger(level)
	devices = []
	peri_name = "all"
	bitfield_pattern = ""
	
	for arg in sys.argv[1:]:
		if arg in ['error', 'warn', 'info', 'debug', 'disabled']:
			level = arg
			logger.setLogLevel(level)
			continue
		
		if "ATtiny" in arg or "ATmega" in arg or 'AT90' in arg:
			xml_path = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'AVR_devices', (arg + '*'))
			files = glob.glob(xml_path)
			for file in files:
				# deal with this here, rather than rewrite half the name merging
				if os.path.basename(file) != "ATtiny28.xml":
					part = AVRDeviceReader(file, logger)
					devices.append(Device(part, logger))
			continue
		
		if any(arg.startswith(per) for per in ["EXT", "TWI", "USART", "SPI", "AD_CON", "USB", "CAN", "DA_CON", "USI", "TIMER"]):
			peri_name = arg
			continue
		
		bitfield_pattern = arg
	
	logger.setLogLevel('debug')
	
	peripherals = []
	for dev in devices:
		attributes = dev.getAttributes('peripherals')
		for attribute in attributes:
			for peripheral in [p for p in attribute['value'] if p.name.startswith(peri_name)]:
				peripherals.append({'ids': [dev.id], 'peripheral': peripheral})
	
	registers = []
	for peri in peripherals:
		for reg in peri['peripheral'].registers:
			registers.append({'ids': list(peri['ids']), 'register': reg})
	
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
			current['ids'].extend(match['ids'])
			registers.remove(match)
		
		if len(matches) == 0:
			logger.warn("No match for register: " + current['register'].name + " of " + str([id.string for id in current['ids']]))
		
		merged.append(current)
	
	filtered_devices = []
	filtered_registers = []
	all_names = []
	
	for dev in merged:
		reg = dev['register']
		dev['ids'].sort(key=lambda k : (int(k.name or 0), k.type))
		all_names.extend([id.string for id in dev['ids']])
		if bitfield_pattern == "":
			filtered_registers.append(dev['register'].name)
			s = "Devices:\n"
			ii = 0
			for id in dev['ids']:
				s += id.string.replace("at","") + "  \t"
				ii += 1
				if ii > 7:
					ii = 0
					s += "\n"
			logger.debug(s)
			logger.info(str(reg) + "\n")
		
		if reg.getFieldsWithPattern(bitfield_pattern) != None:
			filtered_registers.append(dev['register'].name)
			filtered_devices.append(dev)
	
	all_filtered_names = []
	if bitfield_pattern != "":
		logger.info("Registers containing BitField pattern '" + bitfield_pattern + "':")
		for dev in filtered_devices:
			all_filtered_names.extend([id.string for id in dev['ids']])
			s = "Devices:\n"
			ii = 0
			for id in dev['ids']:
				s += id.string.replace("at","") + "  \t"
				ii += 1
				if ii > 7:
					ii = 0
					s += "\n"
			logger.debug(s)
			logger.info(str(dev['register']) + "\n")
	
	filtered_registers = list(set(filtered_registers))
	filtered_registers.sort()
	
	logger.info("Summary registers:")
	for name in filtered_registers:
		logger.debug(name)
	logger.info("Remaining devices:")
	all_names = set(all_names) - set(all_filtered_names)
	all_names = list(all_names)
	all_names.sort()
	s = "\n"
	ii = 0
	for id in all_names:
		s += id.replace("at","") + "  \t"
		ii += 1
		if ii > 7:
			ii = 0
			s += "\n"
	logger.debug(s)

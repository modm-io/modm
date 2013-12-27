#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

import os, sys
from device import Device
from stm_reader import STMDeviceReader
from stm_writer import STMDeviceWriter
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
	
	for arg in sys.argv[1:]:
		if arg in ['error', 'warn', 'info', 'debug', 'disabled']:
			level = arg
			logger.setLogLevel(level)
			continue
		xml_path = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'STM_devices', 'Memory', (arg + '*'))
		files = glob.glob(xml_path)
		for file in files:
			part = STMDeviceReader(file, logger)
			devices.append(Device(part, logger))
	
	merger = DeviceMerger(devices, logger)
	merger.mergedByPlatform('stm32')
	
	folder = os.path.join(os.path.dirname(__file__), '..', '..', 'src', 'xpcc', 'architecture', 'platform', 'xml', 'stm32')
	
	for dev in merger.mergedDevices:
		writer = STMDeviceWriter(dev, logger)
		writer.write(folder)


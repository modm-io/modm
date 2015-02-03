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
	supported_families = ['STM32F0', 'STM32F1', 'STM32F2', 'STM32F3', 'STM32F4']
	family = None

	for arg in sys.argv[1:]:
		if arg in ['error', 'warn', 'info', 'debug', 'disabled']:
			level = arg
			logger.setLogLevel(level)
			continue
		if arg in supported_families:
			family = arg
		else:
			logger.error("Invalid family! Valid input is '{}', found '{}'".format(", ".join(supported_families), arg))
			exit(1)

	if family is None:
		logger.error("Please provide a device family: '{}'".format(", ".join(supported_families)))
		exit(1)

	devicesFromFamily = STMDeviceReader.getDevicesFromFamily(family, logger)

	for deviceName in devicesFromFamily:
		device = STMDeviceReader(deviceName, logger)
		devices.append(Device(device, logger))

	merger = DeviceMerger(devices, logger)
	merger.mergedByPlatform('stm32')

	folder = os.path.join(os.path.dirname(__file__), '..', '..', 'src', 'xpcc', 'architecture', 'platform', 'devices', 'stm32')

	for dev in merger.mergedDevices:
		writer = STMDeviceWriter(dev, logger)
		writer.write(folder)


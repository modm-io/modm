#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, 2016, Fabian Greif
# Copyright (c) 2012, David Hebbeker
# Copyright (c) 2012-2013, 2015-2016, Niklas Hauser
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import sys

# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))

from logger import Logger

from dfg.device import Device
from dfg.merger import DeviceMerger
from dfg.stm32.stm_reader import STMDeviceReader
from dfg.stm32.stm_writer import STMDeviceWriter

if __name__ == "__main__":
	"""
	Some test code
	"""
	level = 'info'
	logger = Logger(level)
	devices = []
	supported_families = ['STM32F0', 'STM32F1', 'STM32F2', 'STM32F3', 'STM32F4', 'STM32F7']
	filtered_family = None
	filtered_device = None

	for arg in sys.argv[1:]:
		if arg in ['error', 'warn', 'info', 'debug', 'disabled']:
			level = arg
			logger.setLogLevel(level)
			continue
		if any (arg.startswith(f) for f in supported_families):
			filtered_device = arg
			filtered_family = arg[:7]
		else:
			logger.error("Invalid family! Valid input is '{}', found '{}'".format(", ".join(supported_families), arg))
			exit(1)

	if filtered_device is None:
		logger.error("Please provide a device family: '{}'".format(", ".join(supported_families)))
		exit(1)

	devicesFromFamily = STMDeviceReader.getDevicesFromFamily(filtered_family, logger)

	for deviceName in [d for d in devicesFromFamily if d.startswith(filtered_device)]:
		device = STMDeviceReader(deviceName, logger)
		devices.append(Device(device, logger))

	merger = DeviceMerger(devices, logger)
	merger.mergedByPlatform('stm32')

	folder = os.path.join(os.path.dirname(__file__), '..', '..', 'src', 'modm', 'architecture', 'platform', 'devices', 'stm32')

	for dev in merger.mergedDevices:
		writer = STMDeviceWriter(dev, logger)
		writer.write(folder)

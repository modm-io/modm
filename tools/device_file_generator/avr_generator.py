#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, 2016, Fabian Greif
# Copyright (c) 2012, David Hebbeker
# Copyright (c) 2012-2013, 2015, Niklas Hauser
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
import glob

# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'device_files'))

from logger import Logger

from dfg.device import Device
from dfg.merger import DeviceMerger
from dfg.avr.avr_reader import AVRDeviceReader
from dfg.avr.avr_writer import AVRDeviceWriter

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
		xml_path = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'AVR_devices', (arg + '*'))
		files = glob.glob(xml_path)
		for file in files:
			# deal with this here, rather than rewrite half the name merging
			if os.path.basename(file) != "ATtiny28.xml":
				part = AVRDeviceReader(file, logger)
				device = Device(part, logger)
				devices.append(device)

	merger = DeviceMerger(devices, logger)
	merger.mergedByPlatform('avr')

	folder = os.path.join(os.path.dirname(__file__), '..', '..', 'src', 'modm', 'architecture', 'platform', 'devices', 'avr')

	for dev in merger.mergedDevices:
		writer = AVRDeviceWriter(dev, logger)
		writer.write(folder)


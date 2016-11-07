#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2013, Kevin Laeufer
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
from device import DeviceFile
from driver import DriverFile


if __name__ == "__main__":
	"""
	Some test code
	"""
	p_dir = os.path.abspath("../../src/xpcc/architecture/platform")

	# Test Device File Class
	dev = DeviceFile("../../src/xpcc/architecture/platform/xml/stm32f40.xml")
	print "dev.getProperties('stm32f407vg'): %s" % dev.getProperties('stm32f407vg')
	print "dev.getProperties('stm32f407ve'): %s" % dev.getProperties('stm32f407ve')
	print "dev.getDriverList('stm32f407vg', a_dir):"
	dList = dev.getDriverList('stm32f407vg', p_dir)
	i = 1
	for d in dList:
		print "%s) %s/%s" % (i, d['type'], d['name'])
		print "path:        %s" % (d['path'])
		print "driver_file: %s" % (d['driver_file'])
		print "instances:"
		print str(d['instances'])
		print "substitutions:"
		print str(d['substitutions'])
		print ""
		i += 1

	# Test DriverFile Class
	for d in dList:
		df = DriverFile.fromDict(d)
		print "df.getBuildList():"
		print str(df.getBuildList(p_dir))
		print ""
		print ""


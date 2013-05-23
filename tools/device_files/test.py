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

import os
from device import DeviceFile


if __name__ == "__main__":
	"""
	Some test code
	"""
	dev = DeviceFile("../../src/xpcc/architecture/platform/xml/stm32f40.xml")
	p_dir = os.path.abspath("../../src/xpcc/architecture/platform")
	"""
	build = dev.getBuildList(os.path.join(a_dir, 'platform'), 'stm32f407vg', os.path.join(a_dir, 'platform_stm32f407vg.generated'))
	for f in build:
		if len(f) == 2:
			print "static:   %s => %s" % (f[0], f[1])
		elif len(f) == 3:
			print "template: %s => %s" % (f[0], f[1])
			print "substitutions: %s" % f[2]
	"""
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

#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright (c) 2011, Roboterclub Aachen e.V.
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
# 
# $Id$
# -----------------------------------------------------------------------------

import os
import re
import sys
import subprocess

COLOR_GREEN = ";31"
COLOR_RED = ";32"
COLOR_YELLOW = ";33"
COLOR_DEFAULT = ""

def setColor(color):
	sys.stdout.write("\033[0%sm" % color)

results = {}
for line in open("regression.txt"):
	path, flash, ram = line.strip().split(" ")
	results[path] = [int(flash), int(ram)]

flashFilter = re.compile('\nProgram:\s+(\d+)')
ramFilter = re.compile('\nData:\s+(\d+)')

newData = []

for path, directories, files in os.walk('../examples'):
	# exclude the SVN-directories
	if '.svn' in directories:
		directories.remove('.svn')
	
	if 'SConstruct' in files:
		cmd = ['scons', '-C%s' % path, 'size']
		p = subprocess.Popen(cmd, stdout=subprocess.PIPE)
		stdout, stderr = p.communicate()
		
		print "check: %s" % path
		if stderr is not None:
			setColor(COLOR_YELLOW)
			print "ERROR: Failure when compiling '%s':" % path
			print stderr
			setColor(COLOR_DEFAULT)
		else:
			try:
				flash = int(flashFilter.search(stdout).group(1))
				ram = int(ramFilter.search(stdout).group(1))
				
				newData.append([path, flash, ram])
				
				try:
					oldFlash, oldRam = results[path]
					divFlash = flash - oldFlash
					divRam = ram - oldRam
					percentFlash = 0 if oldFlash == 0 else divFlash * 100.0 / oldFlash
					precentRam = 0 if oldRam == 0 else divRam * 100.0 / oldRam
					
					if (oldFlash < flash) or (oldRam < ram):
						setColor(COLOR_GREEN)
						print "WARNING: Regression at %s" % path
						if oldFlash != flash:
							print "  Flash : %i -> %i (%+i => %i%%)" % (oldFlash, flash, divFlash, percentFlash)
						if oldRam != ram:
							print "  RAM   : %i -> %i (%+i => %i%%)" % (oldRam, ram, divRam, precentRam)
						setColor(COLOR_DEFAULT)
					elif (oldFlash > flash) or (oldRam > ram):
						setColor(COLOR_RED)
						print "IMPROVEMENT at %s" % path
						if oldFlash != flash:
							print "  Flash : %i -> %i (%+i => %i%%)" % (oldFlash, flash, divFlash, percentFlash)
						if oldRam != ram:
							print "  RAM   : %i -> %i (%+i => %i%%)" % (oldRam, ram, divRam, precentRam)
						setColor(COLOR_DEFAULT)
				except KeyError:
					print "WARNING: not found in previous version!"
			except AttributeError:
				setColor(COLOR_YELLOW)
				print "WARNING: could not gather results!"
				setColor(COLOR_DEFAULT)
				pass

file = open("regression.new.txt", "w")
for result in newData:
	file.write("%s %s %s\n" % (result[0], result[1], result[2]))
file.close()

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
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
import re
import sys
import subprocess
import optparse
import locale

COLOR_GREEN = ";32"
COLOR_RED = ";31"
COLOR_YELLOW = ";33"
COLOR_DEFAULT = ""

def set_color(color):
	sys.stdout.write("\033[0%sm" % color)

def check_and_print_results(old, new):
	"""
	Returns True if the size differes compared to the old version
	"""
	flash, ram = new
	oldFlash, oldRam = old
	
	divFlash = flash - oldFlash
	divRam = ram - oldRam
	percentFlash = 0 if oldFlash == 0 else divFlash * 100.0 / oldFlash
	precentRam = 0 if oldRam == 0 else divRam * 100.0 / oldRam
	
	if (oldFlash < flash) or (oldRam < ram):
		set_color(COLOR_RED)
		print("WARNING: Regression at %s" % path)
		if oldFlash != flash:
			print("  Flash : %i -> %i (%+i => %i%%)" % (oldFlash, flash, divFlash, percentFlash))
		if oldRam != ram:
			print("  RAM   : %i -> %i (%+i => %i%%)" % (oldRam, ram, divRam, precentRam))
		set_color(COLOR_DEFAULT)
		return True
		
	elif (oldFlash > flash) or (oldRam > ram):
		set_color(COLOR_GREEN)
		print("IMPROVEMENT at %s" % path)
		if oldFlash != flash:
			print("  Flash : %i -> %i (%+i => %i%%)" % (oldFlash, flash, divFlash, percentFlash))
		if oldRam != ram:
			print("  RAM   : %i -> %i (%+i => %i%%)" % (oldRam, ram, divRam, precentRam))
		set_color(COLOR_DEFAULT)
		return True
	
	return False

def check_symbols(file_old_symbols, file_new_symbols):
	try:
		old_symbols = {}
		for line in open(file_old_symbols).readlines():
			m = re.match("\d{8} (\d{8}) (\w) (.+)", line)
			if m:
				type = m.group(2)
				name = m.group(3)
				size = int(m.group(1))
				
				# some symbols may appear multiple times
				value = old_symbols.get(name, [])
				value.append((type, size))
				old_symbols[name] = value
		
		for line in open(file_new_symbols).readlines():
			m = re.match("\d{8} (\d{8}) (\w) (.+)", line)
			if m:
				type = m.group(2)
				name = m.group(3)
				size = int(m.group(1))
				
				try:
					values = old_symbols.pop(name)
					if len(values) > 1:
						# find best match
						index = 0
						for (i, (old_type, old_size)) in enumerate(values):
							if (type == old_type and size == old_size):
								index = i
						
						(old_type, old_size) = values[index]
						del values[index]
						
						# restore other symbols
						old_symbols[name] = values
					else:
						(old_type, old_size) = values[0]
					
					if old_size > size:
						set_color(COLOR_GREEN)
						print("    %c %4i -> %4i (%4i) %s" % (type, old_size, size, (size - old_size), name))
						set_color(COLOR_DEFAULT)
						
					elif old_size < size:
						set_color(COLOR_RED)
						print("    %c %4i -> %4i (%4i) %s" % (type, old_size, size, (size - old_size), name))
						set_color(COLOR_DEFAULT)
				except KeyError:
					# key was not found in the old symbol table => new symbol
					set_color(COLOR_RED)
					print("    %c %4i added          %s" % (type, size, name))
					set_color(COLOR_DEFAULT)
		
		for name, values in old_symbols.items():
			# symbol was not removed => symbol is not in the new set
			for value in values:
				(type, size) = value
				
				set_color(COLOR_GREEN)
				print("    %c %4i removed        %s" % (type, size, name))
				set_color(COLOR_DEFAULT)
	except IOError:
		pass

def update_files():
	"""
	Save current status as reference for the next run
	
	- Remove all *.txt files
	- Rename *.2.txt -> *.txt
	"""
	remove = ["regression.txt"]
	rename = [("regression.2.txt", "regression.txt")]
	
	for path, directories, files in os.walk('regression'):
		for file in files:
			if file.endswith(".2.txt"):
				# *.2.txt -> *.txt
				rename.append((os.path.join(path, file),
							   os.path.join(path, file[:-6] + ".txt")))
			elif file.endswith(".txt"):
				remove.append(os.path.join(path, file))
	
	for file in remove:
		try:
			os.remove(file)
		except OSError:
			pass
	
	for (old, new) in rename:
		try:
			os.rename(old, new)
		except OSError:
			pass

if __name__ == '__main__':
	parser = optparse.OptionParser()
	parser.add_option("-v", "--verbose", action="store_true", dest="verbose")
	parser.add_option("-u", "--update", action="store_true", dest="update")
	
	(options, args) = parser.parse_args()
	
	if len(args) == 0:
		dir = '../examples'
	else:
		dir = args[0]
	
	results = {}
	try:
		for line in open("regression.txt"):
			path, flash, ram = line.strip().split(" ")
			results[path] = [int(flash), int(ram)]
	except IOError:
		pass
	
	newData = []

	#for path, directories, files in os.walk('../examples'):
	for path, directories, files in os.walk(dir):
		if 'SConstruct' in files:
			cmd = ['scons', '-C%s' % path, 'size']
			p = subprocess.Popen(cmd, stdout=subprocess.PIPE)
			stdout, stderr = p.communicate()
			
			print("check: %s" % path)
			if stderr is not None:
				set_color(COLOR_YELLOW)
				print("ERROR: Failure when compiling '%s':" % path)
				print(stderr)
				set_color(COLOR_DEFAULT)
			else:
				try:
					result = stdout.decode(locale.getpreferredencoding())
					flash = int(re.search("\nProgram:\s+(\d+)", result).group(1))
					ram = int(re.search("\nData:\s+(\d+)", result).group(1))
					
					newData.append([path, flash, ram])
					
					uid = "_".join(path.split("/")[2:])
					symbols_new = "regression/%s.2.txt" % uid
					symbols_old = "regression/%s.txt" % uid
					os.system("scons -C%s symbols > %s" % (path, symbols_new))
					
					try:
						if check_and_print_results(results[path], (flash, ram)):
							if options.verbose:
								# size differs, do detailed analysis
								check_symbols(symbols_old, symbols_new)
					except KeyError:
						print("WARNING: not found in previous version!")
				except AttributeError:
					set_color(COLOR_YELLOW)
					print("WARNING: could not determine size!")
					set_color(COLOR_DEFAULT)

	file = open("regression.2.txt", "w")
	for result in newData:
		file.write("%s %s %s\n" % (result[0], result[1], result[2]))
	file.close()
	
	# save current status as reference for the next run
	if options.update:
		update_files()

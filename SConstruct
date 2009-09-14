#!/usr/bin/env python
#
# $Id$

import ConfigParser

class OptionParser:
	
	def __init__(self, filename):
		self.parser = ConfigParser.ConfigParser()
		self.parser.read(filename)
	
	def getOptions(self, commandlineArguments):
		""" Returns a dictionary with key's of the form
		<section>.<option> and the values.
		"""
		result = { 'build': {} }
		for section in self.parser.sections():
			result[section] = {}
			for option in self.parser.options(section):
				result[section][option] = self.parser.get(section, option)
		result['build'].update(commandlineArguments)
		return result

options = OptionParser('config.ini').getOptions(ARGLIST)


if options['build']['target'] == 'pc':
	tool = 'pc'
else:
	tool = 'avr'

env = Environment(
			OPTIONS=options,
			tools=[tool],
			toolpath=['misc/scons/'])

SConscript('src/SConscript',
			src='src',
			variant_dir='build', 
			exports='env', 
			duplicate=False)

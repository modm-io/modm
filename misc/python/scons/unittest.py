#!/usr/bin/env python
# 
# $Id$

import re

class Scanner:
	# FIXME correct handling of strings and comments
	def __init__(self, text):
		self.text = text
		self.commentFilter = re.compile(r"/\*(.|[\r\n])*?\*/")
	
	def stripCommentsAndStrings(self):
		self._prepareString()
		self._stripComments()
		
		lines = self.text.splitlines()
		for i, line in enumerate(lines):
			lines[i] = self._stripCppComments(line)
		
		self.text = '\n'.join(lines)
	
	def _prepareString(self):
		self.text = self.text.replace('\r\n', '\n')
		self.text = self.text.replace('\\\n', '')
	
	def _stripCppComments(self, line):
		index = line.find(r"//")
		if index >= 0:
			return line[:index]
		return line
	
	def _stripComments(self):
		self.text = self.commentFilter.sub('', self.text)

if __name__ == '__main__':
	import sys
	
	filename = sys.argv[1]
	input = open(filename).read()
	
	scanner = Scanner(input) 
	scanner.stripCommentsAndStrings()
	
	print scanner.text

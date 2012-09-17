#!/usr/bin/env python

import re
import os
import string
import codecs		# utf-8 file support
from optparse import OptionParser

VERSION = "0.1"

def _replace(match):
	image = match.group('image')
	thumb = "%s.thumb%s" % os.path.splitext(image)
	try:
		title = match.group('title')
	except IndexError, e:
		title = None
	
	if title is None:
		title = ""
	
	substitute = {
		'orig': image,
		'thumb': thumb,
		'title': title
	}
	str = '<div /><div><a href="$orig" class="highslide" onclick="return hs.expand(this)" title="$title"><img src="$thumb" class="screenshot" alt="$title" /></a></div>'
	
	return string.Template(str).substitute(substitute)

class Processor:
	def __init__(self, infile, outfile):
		self.infile = codecs.open(args[0], 'r', 'utf8')
		self.outfile = codecs.open(args[1], 'w', 'utf8')
		
	def process(self):
		p = re.compile(r'<screenshot\s+href="(?P<image>.*?)"(|\s+title="(?P<title>.*?)")(\s+.*?)*? />')
		for line in self.infile:
			line = p.sub(_replace, line)
			line = re.sub('<clear />', '<div style="clear: both;" />', line)
			self.outfile.write(line)
		
		self.outfile.close()


if __name__ == '__main__':
	optparser = OptionParser(
			usage   = "%prog [options] IN OUT",
			version = "%prog version: " + VERSION)
	
	(options, args) = optparser.parse_args()
	
	if len(args) != 2:
		raise BuilderException("You have to specify a valid input and output file")
		exit(1)
	
	processor = Processor(args[0], args[1])
	processor.process()
	

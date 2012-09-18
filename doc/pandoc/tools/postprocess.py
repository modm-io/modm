#!/usr/bin/env python

import re
import os
import string
import codecs		# utf-8 file support
from optparse import OptionParser

VERSION = "0.1"

template_breadcrump = string.Template('<a href="$path">$title</a>')
template_breadcrump_navigation = string.Template('<div id="toc">Path: $navigation</div>')
template_screenshot = string.Template('<div /><div><a href="$orig" class="highslide" onclick="return hs.expand(this)" title="$title"><img src="$thumb" class="screenshot" alt="$title" /></a></div>')

class Processor:
	def __init__(self, infile, outfile, prefix, source):
		if infile.startswith(prefix):
			self.filename = infile[len(prefix):]
		else:
			self.filename = infile
		self.depth = len(self.filename.split("/"))
		self.infile = codecs.open(infile, 'r', 'utf8')
		self.outfile = codecs.open(outfile, 'w', 'utf8')
		self.sourcepath = source
	
	def process(self):
		p = re.compile(r'<screenshot\s+href="(?P<image>.*?)"(|\s+title="(?P<title>.*?)")(\s+.*?)*? />')
		for line in self.infile:
			line = p.sub(self._screenshot, line)
			line = re.sub('<clear />', '<div style="clear: both;" />', line)
			line = re.sub('<breadcrumb />', self._breadcrump, line)
			line = re.sub('(href=)"~/([^"]*?)"', self._link, line)
			line = re.sub('(src=)"~/([^"]*?)"', self._link, line)
			self.outfile.write(line)
		
		self.outfile.close()
		
	def _screenshot(self, match):
		image = self._adjust_url(match.group('image'))
		thumb = "%s.thumb%s" % os.path.splitext(image)
		try:
			title = match.group('title')
		except IndexError, e:
			title = None
		
		if title is None:
			title = ""
		
		return template_screenshot.substitute({
				'orig': image,
				'thumb': thumb,
				'title': title
			})
	
	def _breadcrump(self, match):
		if self.filename.endswith(".html"):
			self.filename = self.filename[:-len(".html")]
		
		str = []
		path = []
		parts = self.filename.split("/")
		for part in parts[:-1]:
			path.append(part)
			
			filepath = "/".join(path)
			
			# Get Title of the file
			file = "%s.md" % filepath
			titleline = codecs.open(os.path.join(self.sourcepath, file), 'r', 'utf8').readline().rstrip()
			m = re.match("^%\s+(.*)$", titleline)
			if m:
				title = m.group(1)
			else:
				title = part
			
			str.append(template_breadcrump.substitute({
					'path': "%s.html" % self._adjust_url(filepath),
					'title': title
				}))
		
		str.insert(0, template_breadcrump.substitute({
					'path': "%s.html" % self._adjust_url("mainpage"),
					'title': "Home"
				}))
		
		return template_breadcrump_navigation.substitute({
				'navigation': " &raquo; ".join(str)
			})
	
	def _link(self, match):
		return '%s"%s"' % (match.group(1), self._adjust_url(match.group(2)))
	
	def _adjust_url(self, input):
		return "../" * (self.depth - 1) + input

if __name__ == '__main__':
	optparser = OptionParser(
			usage   = "%prog [options] IN OUT",
			version = "%prog version: " + VERSION)
		
	optparser.add_option(
				"-p", "--prefix_path",
				dest = "prefix",
				default = ".",
				help = "Path to remove when generating internal links")
	
	optparser.add_option(
				"-s", "--source_folder",
				dest = "source",
				default = ".",
				help = "Source folder of the original markdown files")
	
	(options, args) = optparser.parse_args()
	
	if len(args) != 2:
		raise BuilderException("You have to specify a valid input and output file")
		exit(1)
	
	if not options.prefix.endswith("/"):
		options.prefix += "/"
	if not options.source.endswith("/"):
		options.source += "/"
	
	processor = Processor(args[0], args[1], options.prefix, options.source)
	processor.process()
	

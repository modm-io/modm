#!/usr/bin/env python
#
# $Id$

import os
import time
import jinja2

generation = """/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.tmpl file instead and re-run the
 * script 'generate.py'.
 *
 * Generated %s
 */
// ----------------------------------------------------------------------------""" \
	% time.strftime("%d %b %Y, %H:%M:%S", time.localtime())

env = jinja2.Environment(loader=jinja2.FileSystemLoader(os.getcwd()))

globals = {
	'generation_block': generation
}

for port in ['C', 'D', 'E', 'F']:
	for number in range(0,2):
		id = "%c%i" % (port, number)
		
		print id.lower()
		return
		
		#template = env.get_template('uart.hpp.tmpl', globals=globals)
		#output = template.render(id=id)
		#open('uart_%s.hpp' % id.lower(), 'w').write(output)
		
		#template = env.get_template('uart.cpp.tmpl', globals=globals)
		#output = template.render(id=id)
		#open('uart_%s.cpp' % id.lower(), 'w').write(output)


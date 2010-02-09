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

for number in range(0,4):
	template = env.get_template('uartn.hpp.tmpl', globals=globals)
	output = template.render(id=number)
	open('uart%i.hpp' % number, 'w').write(output)
	
	template = env.get_template('uartn.cpp.tmpl', globals=globals)
	output = template.render(id=number)
	open('uart%i.cpp' % number, 'w').write(output)


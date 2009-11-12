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
// ----------------------------------------------------------------------------
""" % time.strftime("%d %b %Y, %H:%M:%S", time.localtime())

env = jinja2.Environment(loader=jinja2.FileSystemLoader(os.getcwd()))

globals = {
	'generation_block': generation
}

for number in range(0,4):
	template = env.get_template('buffered_uartn.hpp.tmpl', globals=globals)
	output = template.render(id=number)
	open('buffered_uart%i.hpp' % number, 'w').write(output)
	
	template = env.get_template('buffered_uartn_receive.cpp.tmpl', globals=globals)
	output = template.render(id=number)
	open('buffered_uart%i_receive.cpp' % number, 'w').write(output)
	
	template = env.get_template('buffered_uartn_transmit.cpp.tmpl', globals=globals)
	output = template.render(id=number)
	open('buffered_uart%i_transmit.cpp' % number, 'w').write(output)

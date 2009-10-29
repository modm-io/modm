#!/usr/bin/env python
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Roboterclub Aachen e.V. nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
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

import os
import os.path
import glob
import re

import SCons.Builder

def HeaderParse(file_contents):
	"""
	Parse a header file and return a dictionary of all functions
	"""
	data = {}
	
	return data

def HeaderScan(node, env, path):
	"""
	Doxygen Doxyfile source scanner.  This should scan the Doxygen file and add
	any files used to generate docs to the list of source files.
	"""
	print "HeaderScan"
	print node
	print env
	print path
	
	sources = []
	sources = map( lambda path: env.File(path), sources )
	return sources

def HeaderScanCheck(node, env):
	""" Check if we should scan this file"""
	print "Check"
	print node
	
	return os.path.isfile(node.path)

def TestEmitter(source, target, env):
	print "Emitter"
	print source
	print target
	
	targets = []
	"""out_dir = data.get("OUTPUT_DIRECTORY", ".")
	
	# add our output locations
	for (k, v) in output_formats.items():
		if data.get("GENERATE_" + k, v[0]) == "YES":
			targets.append(env.Dir( os.path.join(out_dir, data.get(k + "_OUTPUT", v[1]))) )
	
	# don't clobber targets
	for node in targets:
		env.Precious(node)
	
	# set up cleaning stuff
	for node in targets:
		env.Clean(node, node)
	"""
	return (targets, source)

def generate(env):
	"""
	Add builders and construction variables for the Unittest tool.
	"""
	print "Generate"
	
	unittest_scanner = env.Scanner(
		HeaderScan,
		"HeaderScan",
		scan_check = HeaderScanCheck,
	)
	
	unittest_builder = SCons.Builder.Builder(
		action = "cd $SOURCE",
		emitter = TestEmitter,
		source_factory = env.fs.Dir,
#		target_factory = env.fs.Entry,
		single_source = True,
		source_scanner =  unittest_scanner,
	)
	
	env.Append(BUILDERS = {
		'Unittest': unittest_builder,
	})

def exists(env):
	return True

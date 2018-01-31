#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2018, Raphael Lehmann
# All rights reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

"""
# xpcc CMake wrapper generator

This tool makes it possible to automatically generate a CMakeLists.txt for
applications using xpcc.
"""


import os

def cmake_wrapper_method(env, file_scanner, additional_sources=[]):
	project_path = env['XPCC_BASEPATH']
	files = [os.path.relpath(str(ff), project_path)
	         for ff in file_scanner.header + file_scanner.sources + additional_sources]
	includes = env['CPPPATH']

	# fixme: get the list of available scons targets from scons
	scons_targets = ['size', 'symbols', 'defines', 'build', 'run', 'program', 'fuse', 'openocd-debug',
				  'init', 'lpclink-debug', 'eeprom', 'debug', 'bin', 'dfu', 'listing', 'all']

	# generate CMakeLists.txt file
	proj_path = os.path.join(project_path, 'CMakeLists.txt')
	template_path = os.path.join(env['XPCC_ROOTPATH'], 'templates', 'cmake', 'CMakeLists.txt.in')
	template_substitutions = {
		'includes': includes,
		'files': ' '.join(files),
		'cc': env.subst(env['CC']),
		'cxx': env.subst(env['CXX']),
		'project_name': env['XPCC_PROJECT_NAME'],
		'cflags': ' '.join(env.subst(env['CFLAGS'])),
		'ccflags': ' '.join(env.subst(env['CCFLAGS'])),
		'cxxflags': ' '.join(env.subst(env['CXXFLAGS'])),
		'scons_targets': scons_targets,
		}
	return env.Jinja2Template(target = proj_path, source = template_path, substitutions = template_substitutions)


def generate(env):
	env.AddMethod(cmake_wrapper_method, 'CMakeWrapper')

def exists(env):
	return 1

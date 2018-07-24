#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, Niklas Hauser
# Copyright (c) 2018, Raphael Lehmann
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
# modm CMake wrapper generator

This tool makes it possible to automatically generate a CMakeLists.txt for
applications using modm.
"""

import os

def cmake_wrapper_method(env):
	cwd = env.Dir('#').abspath
	includes = [os.path.relpath(str(ff), cwd) for ff in env['CPPPATH']]

	# fixme: get the list of available scons targets from scons
	scons_targets = ['size', 'symbols', 'defines', 'build', 'run', 'program', 'bin', 'listing', 'all']

	# generate CMakeLists.txt file
	proj_path = os.path.join(cwd, 'CMakeLists.txt')
	template_path = os.path.join(env['BASEPATH'], 'scons', 'site_tools', 'CMakeLists.txt.in')
	template_substitutions = {
		'includes': includes,
		'cc': env.subst(env['CC']),
		'cxx': env.subst(env['CXX']),
		'project_name': env['CONFIG_PROJECT_NAME'],
		'cflags': ' '.join(env.subst(env['CFLAGS'])),
		'ccflags': ' '.join(env.subst(env['CCFLAGS'])),
		'cxxflags': ' '.join(env.subst(env['CXXFLAGS'])),
		'scons_targets': scons_targets,
	}
	return env.Jinja2Template(target=proj_path, source=template_path, substitutions=template_substitutions)


def generate(env):
	env.AddMethod(cmake_wrapper_method, 'CMakeWrapper')

def exists(env):
	return 1

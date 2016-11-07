#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, Kevin Laeufer
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
# xpcc QtCreator Project Tool

This tool makes it possible to automatically generate a QtCreator project for
applications using xpcc.

You need to add something like this to your application's `SConstruct`:

~~~{.py}

files = env.FindFiles('.')

# [...]

env.Alias('qtcreator', env.QtCreatorProject(files))

# [...]

env.Alias('all', ['build', 'qtcreator', 'size'])

~~~
"""


import os

def collect_defines(defines, source):
	if isinstance(source, str):
		defines[source] = None
	elif isinstance(source, dict):
		defines.update(source)
	elif isinstance(source, list):
		[collect_defines(defines, mm) for mm in source]


def qt_creator_project_method(env, file_scanner, additional_sources=[]):
	# collect values
	project_name = env['XPCC_PROJECT_NAME']
	project_path = env['XPCC_BASEPATH']
	defines = {}
	collect_defines(defines, env['CPPDEFINES'])
	collect_defines(defines, file_scanner.defines)
	files = [os.path.relpath(str(ff), project_path)
	         for ff in file_scanner.header + file_scanner.sources + additional_sources]
	includes = env['CPPPATH']

	# generate project files
	proj_path = os.path.join(project_path, project_name)
	temp_path = os.path.join(env['XPCC_ROOTPATH'], 'templates', 'qtcreator', 'project')
	return [
		env.Jinja2Template(
			target = proj_path + '.creator', source = temp_path + '.creator.in',
			substitutions = {}),
		env.Jinja2Template(
			target = proj_path + '.config', source = temp_path + '.config.in',
			substitutions = {'defines': defines}),
		env.Jinja2Template(
			target = proj_path + '.files', source = temp_path + '.files.in',
			substitutions = {'files': files}),
		env.Jinja2Template(
			target = proj_path + '.includes', source = temp_path + '.includes.in',
			substitutions = {'includes': includes})
	]


def generate(env):
	env.AddMethod(qt_creator_project_method, 'QtCreatorProject')

def exists(env):
	return 1

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, Kevin Läufer
# Copyright (c) 2017, Michael Thies
# Copyright (c) 2017-2018, Niklas Hauser
# Copyright (c) 2018, Christopher Durand
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
# modm QtCreator Project Tool

This tool makes it possible to automatically generate a QtCreator project for
applications using modm.

You need to add something like this to your application's `SConstruct`:

~~~{.py}

files = env.FindFiles(".")

# [...]

env.Alias("qtcreator", env.QtCreatorProject(files))

# [...]

env.Alias("all", ["build", "qtcreator", "size"])

~~~
"""

from os.path import join, abspath, relpath

def qt_creator_project_method(env, sources, with_modm=False):
	cwd = env.Dir("#").abspath
	project_name = env["CONFIG_PROJECT_NAME"]
	# preprocessor defines
	defines = {}
	for define in env.get("CPPDEFINES", []):
		if isinstance(define, str):
			define = define.split("=")
		defines[define[0]] = define[1] if len(define) > 1 else ""
	# include paths
	includes = [relpath(i, cwd) for i in env["CPPPATH"]]
	# header files
	headers = []
	if with_modm:
		headers = [relpath(h, cwd) for h in env.FindHeaderFiles("modm")]
		sources += [relpath(s, cwd) for s in env.FindSourceFiles("modm")]

	headers = list(map(str, headers))
	sources = list(map(str, sources))

	# generate project files
	proj_path = join(cwd, project_name)
	temp_path = abspath(join(env["BASEPATH"], "modm", "scons", "site_tools", "qtcreator", "project"))
	return [
		env.Jinja2Template(
			target = proj_path + ".creator",
		    source = temp_path + ".creator.in",
			substitutions = {}),
		env.Jinja2Template(
			target = proj_path + ".config",
			source = temp_path + ".config.in",
			substitutions = {"defines": defines}),
		env.Jinja2Template(
			target = proj_path + ".files",
			source = temp_path + ".files.in",
			substitutions = {"files": sorted(list(set(headers + sources)))}),
		env.Jinja2Template(
			target = proj_path + ".includes",
			source = temp_path + ".includes.in",
			substitutions = {"includes": includes})
	]


def generate(env):
	env.AddMethod(qt_creator_project_method, "QtCreatorProject")

def exists(env):
	return 1

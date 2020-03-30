#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, 2020, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------
from modm_tools import find_files

def generate(env, **kw):
	def find_source_files(env, basepath, ignorePaths=None, ignoreFiles=None):
		return find_files.scan(basepath, find_files.SOURCE, ignorePaths, ignoreFiles)

	def find_header_files(env, basepath, ignorePaths=None, ignoreFiles=None):
		return find_files.scan(basepath, find_files.HEADER, ignorePaths, ignoreFiles)

	def find_any_files(env, basepath, extensions, ignorePaths=None, ignoreFiles=None):
		return find_files.scan(basepath, extensions, ignorePaths, ignoreFiles)

	env.AddMethod(find_any_files, "FindFiles")
	env.AddMethod(find_source_files, "FindSourceFiles")
	env.AddMethod(find_header_files, "FindHeaderFiles")

def exists(env):
	return True

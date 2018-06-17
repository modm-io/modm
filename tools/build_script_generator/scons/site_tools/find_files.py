#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

from pathlib import Path

def listify(node):
	return [node,] if not isinstance(node, list) else node

# -----------------------------------------------------------------------------
class Scanner:

	HEADER = [".h", ".hh", ".hpp"]
	SOURCE = [".cpp", ".cxx", ".cc", ".c", ".sx", ".S", ".s"]

	def __init__(self, env):
		self.env = env
		self.files = []

	def scan(self, basepath, extensions, ignore=None):
		files = set(str(p) for ext in listify(extensions) for p in Path(basepath).glob("**/*{}".format(ext)))
		if ignore is not None:
			files -= set(str(p) for ign in listify(ignore) for p in Path(basepath).glob(ign))
		self.files = sorted(list(files))

	def __str__(self):
		string = "Files: "
		string += "\n  ".join(self.files)
		return string

# -----------------------------------------------------------------------------
def generate(env, **kw):
	def find_files(env, basepath, extensions, ignore=None):
		scanner = Scanner(env)
		scanner.scan(basepath, extensions, ignore)
		return scanner.files

	def find_source_files(env, basepath, ignore=None):
		return find_files(env, basepath, Scanner.SOURCE, ignore)

	def find_header_files(env, basepath, ignore=None):
		return find_files(env, basepath, Scanner.HEADER, ignore)

	env.AddMethod(find_files, "FindFiles")
	env.AddMethod(find_source_files, "FindSourceFiles")
	env.AddMethod(find_header_files, "FindHeaderFiles")

def exists(env):
	return True
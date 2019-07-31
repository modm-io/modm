#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2019, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
from SCons.Script import *
import subprocess
import platform

def run_post_mortem_gdb(target, source, env):
	source = str(source[0])

	artifact = ARGUMENTS.get("firmware", None)
	if artifact is None:
		print("\n> Using the newest firmware may be inaccurate!\n"
		      "> Use 'firmware={hash}' argument to specify a specific firmware.\n")
	else:
		artifact = artifact.lower()
		artifactpath = os.path.join(env["CONFIG_BUILD_BASE"], "artifacts", "{}.elf".format(artifact))
		if os.path.isfile(artifactpath):
			source = artifactpath
		else:
			print("\n> Unable to find artifact '{}' in build cache!\n"
					"> Run without artifact argument to use newest firmware.\n".format(artifact))
			return 1

	crashdebug = "lin64/CrashDebug"
	if "Windows" in platform.platform():
		crashdebug = "win32/CrashDebug.exe"
	elif "Darwin" in platform.platform():
		crashdebug = "osx64/CrashDebug"
	crashdebug = env.subst("$BASEPATH/ext/crashcatcher/bins/{}".format(crashdebug))

	coredump_txt = os.path.relpath(ARGUMENTS.get("coredump", "coredump.txt"))
	if not os.path.isfile(coredump_txt):
		print("\n> Unable to find coredump file!"
		      "\n> Path '{}' is not a file!\n".format(coredump_txt))
		return 1

	config = map(env.subst, env.Listify(env.get("MODM_GDBINIT", [])))
	subst = {
		"config": " ".join(map('-x "{}"'.format, config)),
		"source": source,
		"crashdebug": crashdebug,
		"coredump": coredump_txt,
	}
	cmd = ('arm-none-eabi-gdb {config} {source} -ex "set target-charset ASCII" '
		   '-ex "target remote | {crashdebug} --elf {source} --dump {coredump}"')
	subprocess.call(cmd.format(**subst), cwd=os.getcwd(), shell=True)

	return 0

def gdb_post_mortem_debug(env, source, alias="gdb_post_mortem_debug"):
	action = Action(run_post_mortem_gdb, cmdstr="$POSTMORTEMGDBSTR")
	return env.AlwaysBuild(env.Alias(alias, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
	# build messages
	if not ARGUMENTS.get("verbose"):
		firmware = ARGUMENTS.get("firmware", "")
		if firmware: firmware = " firmware={}".format(firmware);
		fmt = ("{0}.------GDB----> {1}$SOURCE\n"
			   "{0}'---CoreDump--> {2}$CONFIG_DEVICE_NAME{4}{3}"
			   .format("\033[;0;32m", "\033[;0;33m", "\033[;1;33m", "\033[;0;0m", firmware))
		env["POSTMORTEMGDBSTR"] = fmt

	env.AddMethod(gdb_post_mortem_debug, "PostMortemGdb")

def exists(env):
	return env.Detect("arm-none-eabi-gdb")

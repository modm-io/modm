#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2015, Kevin Läufer
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import subprocess
import platform
import getpass
import locale
import re
from collections import defaultdict

def is_git_available():
	return (subprocess.call(['which', 'git'], stdout=open(os.devnull, 'wb')) is 0)

# -----------------------------------------------------------------------------
def git_show(format, ref='HEAD'):
	""" git_show
		returns a string containing the output from git show
	"""
	r = subprocess.check_output(['git', '--no-pager', 'show', ref, '--quiet', '--pretty=format:"' + format + '"'])
	# only use first line, because quiet does not seem to have the expected
	# results for older git versions
	return r.split('\n', 1)[0][1:-1]

def git_config(key):
	""" git_show
		returns a string containing the output from git config
		returns an empty string if the command fails
	"""
	try:
		return subprocess.check_output(['git', 'config', key]).split('\n', 1)[0]
	except subprocess.CalledProcessError:
		return ""

def git_info_defines(env):
	defines = {}
	try:
		# Last Commit Values
		defines['MODM_GIT_SHA']             = git_show('%H')
		defines['MODM_GIT_SHA_ABBR']        = git_show('%h')
		defines['MODM_GIT_AUTHOR']          = git_show('%an')
		defines['MODM_GIT_AUTHOR_EMAIL']    = git_show('%ae')
		defines['MODM_GIT_AUTHOR_DATE']     = git_show('%ad')
		defines['MODM_GIT_AUTHOR_DATE_TIMESTAMP'] = git_show('%at')
		defines['MODM_GIT_COMMITTER']       = git_show('%cn')
		defines['MODM_GIT_COMMITTER_EMAIL'] = git_show('%ce')
		defines['MODM_GIT_COMMITTER_DATE']  = git_show('%cd')
		defines['MODM_GIT_COMMITTER_DATE_TIMESTAMP'] = git_show('%ct')
		defines['MODM_GIT_SUBJECT']         = git_show('%s')
		# Git Config
		defines['MODM_GIT_CONFIG_USER_NAME']  = git_config('user.name')
		defines['MODM_GIT_CONFIG_USER_EMAIL'] = git_config('user.email')
		# Status
		s = subprocess.check_output(['git', '--no-pager', 'status', '--porcelain']).split('\n')
		f = defaultdict(int)
		for line in s:
			if len(line.strip()) > 0:
				c = line.strip()[0]
				f[c] = f[c] + 1
		defines['MODM_GIT_MODIFIED']  = f['M']
		defines['MODM_GIT_ADDED']     = f['A']
		defines['MODM_GIT_DELETED']   = f['D']
		defines['MODM_GIT_RENAMED']   = f['R']
		defines['MODM_GIT_COPIED']    = f['C']
		defines['MODM_GIT_UPDATED_NOT_MERGED'] = f['U']
		defines['MODM_GIT_UNTRACKED'] = f['?']
		defines = {k:"\\\"{}\\\"".format(v) for k, v in defines.items() }
	except subprocess.CalledProcessError as e:
		env.Error('failed to run git command: %s' % e)

	# We're not creating a header file for these, because all files are supposed to be checked
	# in and checking in the header file with the git info utterly defeats the point of this.
	# Instead we're adding these defines via the command line.
	env.Append(CPPDEFINES=defines)

def build_info_defines(env):
	defines = {}
	defines['MODM_BUILD_MACHINE'] = platform.node()
	defines['MODM_BUILD_USER'] = getpass.getuser()
	# Generate OS String
	if platform.system() == 'Linux':
		os = " ".join(platform.linux_distribution())
	elif platform.system() == 'Darwin':
		os = "Mac {0} ({2})".format(*platform.mac_ver())
	else:
		os = platform.system()
	defines['MODM_BUILD_OS'] = os
	# This contains the version of the compiler that is used to build the project
	try:
		c = subprocess.check_output([env['CXX'], '--version']).decode(locale.getpreferredencoding()).split('\n', 1)[0]
	except Exception as e:
		env.Error("[CXX] compiler " + env['CXX'] + " is not in path or could not be executed")
		Exit(1)

	m = re.match("(?P<name>[a-z\-\+]+)[a-zA-Z\(\) ]* (?P<version>\d+\.\d+\.\d+)", c)
	if m: comp = "{0} {1}".format(m.group('name'), m.group('version'))
	else: comp = c
	defines['MODM_BUILD_COMPILER'] = comp
	defines = {k:"\\\"{}\\\"".format(v) for k, v in defines.items() }

	# CPP defines instead of a header files.
	env.Append(CPPDEFINES=defines)


def generate(env, **kw):
	if is_git_available():
		env.AddMethod(git_info_defines, 'InfoGit')
	env.AddMethod(build_info_defines, 'InfoBuild')

def exists(env):
	return True

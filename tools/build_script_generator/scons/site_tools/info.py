#!/usr/bin/env python3
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
from os.path import join, dirname, abspath

TEMPLATE_SOURCE = join(dirname(abspath(__file__)), "info.c.in")

def is_git_available():
	git_exists = subprocess.call(['which', 'git'], cwd=os.getcwd(), stdout=open(os.devnull, 'wb')) is 0
	is_git_repo = subprocess.call(['git', 'status'], cwd=os.getcwd(), stdout=open(os.devnull, 'wb')) is 0
	return git_exists and is_git_repo

# -----------------------------------------------------------------------------
def git_show(cwd, format, ref='HEAD'):
	""" git_show
		returns a string containing the output from git show
	"""
	r = subprocess.check_output(
	        ['git', '--no-pager', 'show', ref, '--quiet', '--pretty=format:"' + format + '"'],
	        cwd=cwd)
	# only use first line, because quiet does not seem to have the expected
	# results for older git versions
	r = r.decode(locale.getpreferredencoding()).split('\n', 1)[0]
	if r.startswith("\""): r = r[1:-1];
	r = r.replace('"', '\\"')
	return r

def git_config(cwd, key):
	""" git_config
		returns a string containing the output from git config
		returns an empty string if the command fails
	"""
	try:
		r = subprocess.check_output(['git', 'config', key], cwd=cwd)
		r = r.decode(locale.getpreferredencoding()).split('\n', 1)[0]
		if r.startswith("\""): r = r[1:-1];
		r = r.replace('"', '\\"')
		return r
	except subprocess.CalledProcessError:
		return ""

def git_info_defines(env, with_status=False):
	cwd = env.Dir('#').abspath
	defines = {}
	subs = {}
	try:
		defines['MODM_GIT_INFO'] = "1"
		# Last Commit Values
		subs['MODM_GIT_SHA']                      = git_show(cwd, '%H')
		subs['MODM_GIT_SHA_ABBR']                 = git_show(cwd, '%h')
		subs['MODM_GIT_AUTHOR']                   = git_show(cwd, '%an')
		subs['MODM_GIT_AUTHOR_EMAIL']             = git_show(cwd, '%ae')
		subs['MODM_GIT_AUTHOR_DATE']              = git_show(cwd, '%ad')
		subs['MODM_GIT_AUTHOR_DATE_TIMESTAMP']    = git_show(cwd, '%at')
		subs['MODM_GIT_COMMITTER']                = git_show(cwd, '%cn')
		subs['MODM_GIT_COMMITTER_EMAIL']          = git_show(cwd, '%ce')
		subs['MODM_GIT_COMMITTER_DATE']           = git_show(cwd, '%cd')
		subs['MODM_GIT_COMMITTER_DATE_TIMESTAMP'] = git_show(cwd, '%ct')
		subs['MODM_GIT_SUBJECT']                  = git_show(cwd, '%s')
		# Git Config
		subs['MODM_GIT_CONFIG_USER_NAME']  = git_config(cwd, 'user.name')
		subs['MODM_GIT_CONFIG_USER_EMAIL'] = git_config(cwd, 'user.email')

		# Status
		if with_status:
			s = subprocess.check_output(['git', '--no-pager', 'status', '--porcelain'], cwd=cwd)
			s = s.decode(locale.getpreferredencoding()).split('\n')
			f = defaultdict(int)
			for line in s:
				if len(line.strip()) > 0:
					c = line.strip()[0]
					f[c] += 1
			defines['MODM_GIT_STATUS'] = "1"
			subs['MODM_GIT_MODIFIED']  = f['M']
			subs['MODM_GIT_ADDED']     = f['A']
			subs['MODM_GIT_DELETED']   = f['D']
			subs['MODM_GIT_RENAMED']   = f['R']
			subs['MODM_GIT_COPIED']    = f['C']
			subs['MODM_GIT_UPDATED_NOT_MERGED'] = f['U']
			subs['MODM_GIT_UNTRACKED'] = f['?']

	except subprocess.CalledProcessError as e:
		env.Error('failed to run git command: %s' % e)
		return

	env.AppendUnique(CPPDEFINES=defines)
	target = join(env['BASEPATH'], 'src', 'info_git.c')
	subs = {"type": "git", "defines": {k:u"\"{}\"".format(v) for k, v in subs.items()}}
	return env.Jinja2Template(target=target, source=TEMPLATE_SOURCE, substitutions=subs)


def build_info_defines(env):
	cwd = env.Dir('#').abspath
	defines = {}
	subs = {}
	defines['MODM_BUILD_INFO'] = "1"
	subs['MODM_BUILD_PROJECT_NAME'] = env.get('CONFIG_PROJECT_NAME', 'Unknown')
	subs['MODM_BUILD_MACHINE'] = platform.node()
	subs['MODM_BUILD_USER'] = getpass.getuser()
	# Generate OS String
	if platform.system() == 'Linux' and "linux_distribution" in dir(platform):
		os = " ".join(platform.linux_distribution())
	elif platform.system() == 'Darwin':
		os = "macOS {0} ({2})".format(*platform.mac_ver())
	else:
		os = platform.platform(terse=True)
	subs['MODM_BUILD_OS'] = os
	# This contains the version of the compiler that is used to build the project
	try:
		c = subprocess.check_output([env['CXX'], '--version'], cwd=cwd)
		c = c.decode(locale.getpreferredencoding()).split('\n', 1)[0]
	except Exception as e:
		env.Error("[CXX] compiler " + env['CXX'] + " is not in path or could not be executed")
		Exit(1)

	m = re.match(r"(?P<name>[a-z\-\+]+)[a-zA-Z\(\) ]* (?P<version>\d+\.\d+\.\d+)", c)
	if m: comp = "{0} {1}".format(m.group('name'), m.group('version'))
	else: comp = c
	subs['MODM_BUILD_COMPILER'] = comp

	env.AppendUnique(CPPDEFINES=defines)
	target = join(env['BASEPATH'], 'src', 'info_build.c')
	subs = {"type": "build", "defines": {k:u"\"{}\"".format(v) for k, v in subs.items()}}
	return env.Jinja2Template(target=target, source=TEMPLATE_SOURCE, substitutions=subs)


def generate(env, **kw):
	if is_git_available():
		env.AddMethod(git_info_defines, 'InfoGit')
	env.AddMethod(build_info_defines, 'InfoBuild')

def exists(env):
	return True

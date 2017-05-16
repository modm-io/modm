#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2015, Kevin Läufer
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import subprocess, os

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

def git_info_header(env):
	defines = {}
	try:
		# Last Commit Values
		defines['MODM_GIT_SHA']             = env.CStringLiteral(git_show('%H'))
		defines['MODM_GIT_SHA_ABBR']        = env.CStringLiteral(git_show('%h'))
		defines['MODM_GIT_AUTHOR']          = env.CStringLiteral(git_show('%an'))
		defines['MODM_GIT_AUTHOR_EMAIL']    = env.CStringLiteral(git_show('%ae'))
		defines['MODM_GIT_AUTHOR_DATE']     = env.CStringLiteral(git_show('%ad'))
		defines['MODM_GIT_AUTHOR_DATE_TIMESTAMP'] = git_show('%at')
		defines['MODM_GIT_COMMITTER']       = env.CStringLiteral(git_show('%cn'))
		defines['MODM_GIT_COMMITTER_EMAIL'] = env.CStringLiteral(git_show('%ce'))
		defines['MODM_GIT_COMMITTER_DATE']  = env.CStringLiteral(git_show('%cd'))
		defines['MODM_GIT_COMMITTER_DATE_TIMESTAMP'] = git_show('%ct')
		defines['MODM_GIT_SUBJECT']         = env.CStringLiteral(git_show('%s'))
		# Git Config
		defines['MODM_GIT_CONFIG_USER_NAME']  = env.CStringLiteral(git_config('user.name'))
		defines['MODM_GIT_CONFIG_USER_EMAIL'] = env.CStringLiteral(git_config('user.email'))
		# Status
		s = subprocess.check_output(['git', '--no-pager', 'status', '--porcelain']).split('\n')
		f = { 'M': 0, 'A': 0, 'D': 0, 'R': 0, 'C': 0, 'U': 0, '?': 0}
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
	except subprocess.CalledProcessError as e:
		env.Error('failed to run git command: %s' % e)
	c = "Its content is created by a call to env.GitInfoHeader() in your SConstruct file."
	env.DefineHeader(defines=defines, header="modm_git_info.hpp", comment=c)

def generate(env, **kw):
	env.AddMethod(git_info_header, 'GitInfoHeader')

def exists(env):
	return is_git_available()

#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2014, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

import subprocess, os

def is_git_available():
	return (subprocess.call(["which git"]))

# -----------------------------------------------------------------------------
def git_show(format, ref='HEAD'):
	""" git_show
		returns a string containing the output from git show
	"""
	r = subprocess.check_output(['git', '--no-pager', 'show', ref, '--quiet', '--pretty=format:"' + format + '"'])
	return r[1:-1]


def git_info_header(env):
	defines = {}
	try:
		# Last Commit Values
		defines['XPCC_GIT_SHA']             = '"{0}"'.format(git_show('%H'))
		defines['XPCC_GIT_SHA_ABBR']        = '"{0}"'.format(git_show('%h'))
		defines['XPCC_GIT_AUTHOR']          = '"{0}"'.format(git_show('%an'))
		defines['XPCC_GIT_AUTHOR_EMAIL']    = '"{0}"'.format(git_show('%ae'))
		defines['XPCC_GIT_AUTHOR_DATE']     = '"{0}"'.format(git_show('%ad'))
		defines['XPCC_GIT_AUTHOR_DATE_TIMESTAMP'] = git_show('%at')
		defines['XPCC_GIT_COMMITTER']       = '"{0}"'.format(git_show('%cn'))
		defines['XPCC_GIT_COMMITTER_EMAIL'] = '"{0}"'.format(git_show('%ce'))
		defines['XPCC_GIT_COMMITTER_DATE']  = '"{0}"'.format(git_show('%cd'))
		defines['XPCC_GIT_COMMITTER_DATE_TIMESTAMP'] = git_show('%ct')
		defines['XPCC_GIT_SUBJECT']         = '"{0}"'.format(git_show('%s'))
		# Status
		s = subprocess.check_output(['git', '--no-pager', 'status', '--porcelain']).split('\n')
		f = { 'M': 0, 'A': 0, 'D': 0, 'R': 0, 'C': 0, 'U': 0, '?': 0}
		for line in s:
			if len(line.strip()) > 0:
				c = line.strip()[0]
				f[c] = f[c] + 1
		defines['XPCC_GIT_MODIFIED']  = f['M']
		defines['XPCC_GIT_ADDED']     = f['A']
		defines['XPCC_GIT_DELETED']   = f['D']
		defines['XPCC_GIT_RENAMED']   = f['R']
		defines['XPCC_GIT_COPIED']    = f['C']
		defines['XPCC_GIT_UPDATED_NOT_MERGED'] = f['U']
		defines['XPCC_GIT_UNTRACKED'] = f['?']
	except subprocess.CalledProcessError as e:
		env.Error('failed to run git command: %s' % e)

	# show scons how to build the git info header file
	# this works just like building the xpcc_config.hpp in the xpcc.py tool
	define_list = ["#define %s %s" % (key.upper(), value) for key, value in defines.iteritems()]
	file = env.Template(
		target = os.path.join(env['XPCC_BUILDPATH'], 'xpcc_git_info.hpp'),
		source = os.path.join(env['XPCC_ROOTPATH'], 'templates', 'xpcc_git_info.hpp.in'),
		substitutions = {'defines': '\n'.join(define_list)})
	env.AppendUnique(CPPPATH = env['XPCC_BUILDPATH'])

def generate(env, **kw):
	env.AddMethod(git_info_header, 'GitInfoHeader')

def exists(env):
	return is_git_available()

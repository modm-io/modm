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

import os
from SCons.Script import *
import traceback
import jinja2

def jinja2_template_action(target, source, env):
	filename = source[0].path
	loader = jinja2.Environment(loader = jinja2.FileSystemLoader(os.path.dirname(filename)))

	# enable '%%' as prefix for line statements
	loader.line_statement_prefix = '%%'
	loader.line_comment_prefix = '%#'

	try:
		template = loader.get_template(os.path.basename(filename))
	except Exception as e:
		traceback.print_exc()
		raise Exception('Failed to retrieve Template', e)

	output = template.render(env['substitutions'])
	with open(target[0].path, 'w', encoding='utf-8') as outfile:
		outfile.write(output)

def template_emitter(target, source, env):
	Depends(target, SCons.Node.Python.Value(env['substitutions']))
	return target, source

def generate(env, **kw):
	env['BUILDERS']['Jinja2Template'] = env.Builder(
		action = Action(jinja2_template_action, cmdstr="$JINJA_TEMPLATE_COMSTR"),
		emitter = template_emitter,
		single_source = True,
		src_suffix = '.in')

def exists(env):
	return True

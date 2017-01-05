#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Kevin Laeufer
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import sys, os, platform
from SCons.Script import *

# add python module from tools to path
# this is apparently not pythonic, but I see no other way to do this
# without polluting the site_tools directory or haveing duplicate code
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools', 'logger'))
from logger import Logger

# -----------------------------------------------------------------------------
def logger_debug(env, s, alias='logger_debug'):
	env['MODM_LOGGER'].debug(s)

# -----------------------------------------------------------------------------
def logger_info(env, s, alias='logger_info'):
	env['MODM_LOGGER'].info(s)

# -----------------------------------------------------------------------------
def logger_warn(env, s, alias='logger_warn'):
	env['MODM_LOGGER'].warn(s)

# -----------------------------------------------------------------------------
def logger_error(env, s, alias='logger_error'):
	env['MODM_LOGGER'].error(s)

# -----------------------------------------------------------------------------
def logger_set_log_level(env, new_level, alias='logger_set_log_level'):
	env['MODM_LOGGER'].setLogLevel(new_level)

# -----------------------------------------------------------------------------
def logger_is_log_level(env, log_level, alias='logger_is_log_level'):
	env['MODM_LOGGER'].isLogLevel(log_level)

# -----------------------------------------------------------------------------
def logger_get_logger(env, alias='logger_is_log_level'):
	return env['MODM_LOGGER']

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env['MODM_LOGGER'] = Logger()
	env.AddMethod(logger_debug, 'Debug')
	env.AddMethod(logger_info,  'Info')
	env.AddMethod(logger_warn,  'Warn')
	env.AddMethod(logger_error, 'Error')
	env.AddMethod(logger_set_log_level, 'SetLogLevel')
	env.AddMethod(logger_is_log_level, 'IsLogLevel')
	env.AddMethod(logger_get_logger, 'GetLogger')

def exists(env):
	return True

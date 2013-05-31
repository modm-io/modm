#!/usr/bin/env python
# 
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Roboterclub Aachen e.V. nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import sys, os, platform
from SCons.Script import *

# add python module from tools to path
# this is apparently not pythonic, but I see no other way to do this
# without polluting the site_tools directory or haveing duplicate code
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools', 'logger'))
from logger import Logger

# -----------------------------------------------------------------------------
def logger_debug(env, s, alias='logger_debug'):
	env['XPCC_LOGGER'].debug(s)

# -----------------------------------------------------------------------------
def logger_info(env, s, alias='logger_info'):
	env['XPCC_LOGGER'].info(s)

# -----------------------------------------------------------------------------
def logger_warn(env, s, alias='logger_warn'):
	env['XPCC_LOGGER'].warn(s)

# -----------------------------------------------------------------------------
def logger_error(env, s, alias='logger_error'):
	env['XPCC_LOGGER'].error(s)

# -----------------------------------------------------------------------------
def logger_set_log_level(env, new_level, alias='logger_set_log_level'):
	env['XPCC_LOGGER'].setLogLevel(new_level)

# -----------------------------------------------------------------------------
def logger_is_log_level(env, log_level, alias='logger_is_log_level'):
	env['XPCC_LOGGER'].isLogLevel(log_level)

# -----------------------------------------------------------------------------
def logger_get_logger(env, alias='logger_is_log_level'):
	return env['XPCC_LOGGER']

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env['XPCC_LOGGER'] = Logger()
	env.AddMethod(logger_debug, 'Debug')
	env.AddMethod(logger_info,  'Info')
	env.AddMethod(logger_warn,  'Warn')
	env.AddMethod(logger_error, 'Error')
	env.AddMethod(logger_set_log_level, 'SetLogLevel')
	env.AddMethod(logger_is_log_level, 'IsLogLevel')
	env.AddMethod(logger_get_logger, 'GetLogger')

def exists(env):
	return True

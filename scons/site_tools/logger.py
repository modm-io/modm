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

class Logger:
	# Terminal Escape Sequences
	COLOR_END     = '\033[0m'
	COLOR_BLACK   = '\033[30m'
	COLOR_RED     = '\033[31m'
	COLOR_GREEN   = '\033[32m'
	COLOR_YELLOW  = '\033[33m'
	COLOR_BLUE    = '\033[34m'
	COLOR_MAGENTA = '\033[35m'
	COLOR_CYAN    = '\033[36m'
	COLOR_WHITE   = '\033[37m'

	# Define Debug Colors
	COLOR_DEBUG   = COLOR_BLUE
	COLOR_INFO    = COLOR_YELLOW
	COLOR_WARN    = COLOR_MAGENTA
	COLOR_ERROR   = COLOR_RED

	# Log Level
	LOG_LEVEL = {
		'debug':   0,
		'info':    1,
		'warning': 2,
		'warn':    2,
		'error':   3,
		'err':     3,
		'disabled': 4 }


	def __init__(self):
		self.stderr_color = self._checkColorSupport(sys.stderr)
		self.stdout_color = self._checkColorSupport(sys.stdout)
		# Default is Error
		self.log_level = self.LOG_LEVEL['err']
		self.line_info = True

	def debug(self, s, format_list):
		if self.log_level <= self.LOG_LEVEL['debug']:
			self.write("Debug: " + s, format_list, self.COLOR_DEBUG, sys.stdout)

	def info(self, s, format_list):
		if self.log_level <= self.LOG_LEVEL['info']:
			self.write("Info: " + s, format_list, self.COLOR_INFO, sys.stdout)

	def warn(self, s, format_list):
		if self.log_level <= self.LOG_LEVEL['warn']:
			self.write("Warn: " + s, format_list, self.COLOR_WARN, sys.stderr)

	def error(self, s, format_list):
		if self.log_level <= self.LOG_LEVEL['error']:
			self.write("Error: " + s, format_list, self.COLOR_ERROR, sys.stderr)

	def write(self, string, format_list=None, color=None, stream=sys.stdout):
		# Check Color Support
		if stream == sys.stdout and not self.stdout_color:
			color = None
		if stream == sys.stderr and not self.stderr_color:
			color = None
		# Print Color
		if color != None:
			stream.write(color)
		# Print String
		if format_list != None:
			stream.write(string % format_list)
		else:
			stream.write(string)
		# End Color
		if color != None:
			stream.write(self.COLOR_END)
		# Line Ending
		stream.write(os.linesep)

	def setLogLevel(self, new_level):
		if isinstance(new_level, 'int'):
			self.log_level = new_level
		elif isinstance(new_level, 'str'):
			new_level = new_level.lower()
			if new_level in self.LOG_LEVEL:
				self.log_level = self.LOG_LEVEL[new_level]

	def _checkColorSupport(self, stream):
		if hasattr(stream, "isatty") and stream.isatty() or \
			('TERM' in os.environ and os.environ['TERM']=='ANSI'):
			return True
		else:
			return False


# -----------------------------------------------------------------------------
def logger_debug(env, s, format_list=None, alias='logger_debug'):
	env['XPCC_LOGGER'].debug(s, format_list)

# -----------------------------------------------------------------------------
def logger_info(env, s, format_list=None, alias='logger_info'):
	env['XPCC_LOGGER'].info(s, format_list)

# -----------------------------------------------------------------------------
def logger_warn(env, s, format_list=None, alias='logger_warn'):
	env['XPCC_LOGGER'].warn(s, format_list)

# -----------------------------------------------------------------------------
def logger_error(env, s, format_list=None, alias='logger_error'):
	env['XPCC_LOGGER'].error(s, format_list)

# -----------------------------------------------------------------------------
def logger_set_log_level(env, new_level, alias='logger_set_log_level'):
	env['XPCC_LOGGER'].setLogLevel(new_level)

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env['XPCC_LOGGER'] = Logger()
	env.AddMethod(logger_debug, 'Debug')
	env.AddMethod(logger_info,  'Info')
	env.AddMethod(logger_warn,  'Warn')
	env.AddMethod(logger_error, 'Error')
	env.AddMethod(logger_set_log_level, 'SetLogLevel')

def exists(env):
	return True

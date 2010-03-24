#!/usr/bin/env python
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
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
# 
# $Id$

import ConfigParser

class ParserException(Exception):
	pass

class XpccConfigParser(ConfigParser.RawConfigParser):
	
	def read(self, filename):
		return ConfigParser.RawConfigParser.read(self, filename)
	
	def get(self, section, option, default=None):
		try:
			return ConfigParser.RawConfigParser.get(self, section, option)
		except (ConfigParser.NoOptionError,
				ConfigParser.NoSectionError), e:
			if default != None:
				return default
			else:
				raise ParserException(e)
	
	def getboolean(self, section, option, default=None):
		try:
			return ConfigParser.RawConfigParser.getboolean(self, section, option)
		except (ConfigParser.NoOptionError,
				ConfigParser.NoSectionError,
				ParserException), e:
			if default != None:
				return default
			else:
				raise ParserException(e)
	
	def items(self, section):
		try:
			return ConfigParser.RawConfigParser.items(self, section)
		except (ConfigParser.NoOptionError,
				ConfigParser.NoSectionError), e:
			raise ParserException(e)

# -----------------------------------------------------------------------------
def generate_configparser(env):
	return XpccConfigParser()

# -----------------------------------------------------------------------------
def generate(env, **kw):
	env.AddMethod(generate_configparser, 'ConfigParser')

def exists(env):
	return True

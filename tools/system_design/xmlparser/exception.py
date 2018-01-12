#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010, Georgi Grinshpun
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

class ParserError(Exception):
	
	def __init__(self, elem, msg):
		""" Emit a error message
		
		elem	- element that reports the error
		msg		- error message
		
		"""
		self.elem = elem
		Exception.__init__(self, msg)
		
	def __str__(self):
		msg = ""
		if self.elem is not None:
			msg += "in %s, " % self.elem
		msg += Exception.__str__(self)
		return msg

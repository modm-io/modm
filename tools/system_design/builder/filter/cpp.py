#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2015, Georgi Grinshpun
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2017, Michael Thies
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

BUILTIN = [	'int8_t', 'int16_t', 'int32_t',
			'uint8_t', 'uint16_t', 'uint32_t',
			'char', 'float' ]

# -----------------------------------------------------------------------------
def typeName(value):
	if value in BUILTIN:
		return value
	return value.title().replace(' ', '')

def variableName(value):
	value = value.title().replace(' ', '')
	value = value[0].lower() + value[1:]
	return value

def enumElement(value):
	return value.upper().replace(' ', '_')

def toHexValue(value):
	if value != None:
		return "0x%02x" % value
	raise ParserException("Value may not be None. Check xml files for missing ids.")

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2010-2011, Fabian Greif
# Copyright (c) 2010, 2013, Georgi Grinshpun
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2017, Michael Thies
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

class Primitive:
	def __init__(self, type, javaType, name, size, equivalent, javaWrapper, accessor, mask):
		self.type = type
		self.javaType = javaType
		self.name = name
		self.size = size
		self.equivalent = equivalent
		self.javaWrapper = javaWrapper
		self.accessor = accessor
		self.mask = mask

	def __cmp__(self, other):
		return 1 - self.__eq__(other) - 2 * self.__lt__(other)

	def __lt__(self, other):
		if isinstance(other, str):
			return self.name < other
		else:
			return self.name < other.name

	def __eq__(self, other):
		if isinstance(other, str):
			return self.name == other
		else:
			return self.name == other.name

PRIMITIVES = {
	"int8_t":	Primitive("int8_t", 	"int",		"Int8",		1,	"byte",		"Byte",		"", 	None),
	"int16_t":	Primitive("int16_t", 	"int",		"Int16",	2,	"short",	"Integer",	"Short",None),
	"int32_t":	Primitive("int32_t", 	"int",		"Int32",	4,	"int", 		"Integer",	"Int",	None),
	"uint8_t":	Primitive("uint8_t", 	"int",		"Uint8",	1,	"byte",		"Integer",	"",		"0xFF"),
	"uint16_t":	Primitive("uint16_t", 	"int",		"Uint16",	2,	"short",	"Integer",	"Short","0xFFFF"),
	"uint32_t":	Primitive("uint32_t", 	"long",		"Uint32",	4,	"int",		"Long",		"Int",	"0xFFFFFFFFl"),
	"float": 	Primitive("float", 		"float",	"Float",	4,	"float",	"Float",	"Float",None),
	"char":		Primitive("char", 		"char",		"Char",		1,	"byte",		"Character",		"",		None),
	"Bool":		Primitive("Bool",		"boolean",	"Bool", 	1,	"boolean",	"Boolean",	"",		None),
}

# -----------------------------------------------------------------------------
def typeJavaObjectName(name):
	if name in PRIMITIVES:
		return PRIMITIVES[name].javaWrapper
	else:
		return name.title().replace(' ', '')

def typeName(name):
	if name in PRIMITIVES:
		return PRIMITIVES[name].javaType
	else:
		return name.title().replace(' ', '')

def typeObjectName(name):
	if name in PRIMITIVES:
		return PRIMITIVES[name].name
	else:
		return name.title().replace(' ', '')

def variableName(name):
	name = name.title().replace(' ', '')
	name = name[0].lower() + name[1:]
	return name

def enumElement(name):
	return name.upper().replace(' ', '_')

def inStringDescription(description):
	return "\\n".join(description.replace('"', '\\"').splitlines());

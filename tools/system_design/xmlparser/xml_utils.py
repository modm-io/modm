#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2011, Fabian Greif
# Copyright (c) 2013, Sascha Schade
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

def get_value(node, key):
	"""  """
	value = node.get(key)
	if value == None:
		try:
			value = node.find(key).text
		except AttributeError:
			pass
	return value

def get_description(node):
	""" Returns the description-tag with trailing tabs removed """
	value = node.get('description')
	if value == None:
		try:
			value = node.find('description').text
		except AttributeError:
			return None
	
	if value == "" or value == None:
		return None
	
	# Entferne die Tabs die durch das Einrücken innerhalb der
	# XML Datei entstanden sind
	str = '\n'.join([line.strip('\t') for line in value.split('\n')])
	str = str.strip('\n')
	
	return str

def get_string(node):
	""" Returns the short string-tag with trailing tabs removed """
	value = node.get('string')
	if value == None:
		try:
			value = node.find('string').text
		except AttributeError:
			return None
		
	if value == "" or value == None:
		return None

	# Entferne die Tabs die durch das Einrücken innerhalb der
	# XML Datei entstanden sind
	str = '\n'.join([line.strip('\t') for line in value.split('\n')])
	str = str.strip('\n')
	
	return str

def get_identifier(node):
	""" Reads the id-tag from xml, converted to an integer """
	try:
		return int(node.get('id'), 0)
	except TypeError:
		return None

#def get_extended(top):
#	"""
#	Parse the an 'extended' tag (if existing)
#	"""
#	node = top.find('extended')
#	if node:
#		extended = node.attrib
#		for subnode in node.getchildren():
#			extended[subnode.tag] = subnode.get('value') or subnode.text
#		return extended
#	else:
#		return {}

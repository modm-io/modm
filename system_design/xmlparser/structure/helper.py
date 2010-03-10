#!/usr/bin/env python
# -*- coding: utf-8 -*-

from exception import ParserError

def xml_read_value(node, key):
	"""  """
	value = node.get(key)
	if value == None:
		try:
			value = node.find(key).text
		except AttributeError:
			pass
	return value

def xml_read_description(node):
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

def xml_read_identifier(node):
	""" Reads the id-tag from xml, converted to an integer """
	try:
		return int(node.get('id'), 0)
	except TypeError:
		return None

def xml_read_extended(top):
	"""
	Parse the an 'extended' tag (if existing)
	"""
	node = top.find('extended')
	if node:
		extended = node.attrib
		for subnode in node.getchildren():
			extended[subnode.tag] = subnode.get('value') or subnode.text
		return extended
	else:
		return {}

def check_name_notation(self, name):
	""" Checks if a string comply with some rules for the notation
	of a name.
	
	"""
	last_low = False
	for c in name:
		if c.isalpha():
			if c.isupper():
				if last_low:
					raise ParserError(self, "Don't use CamelCase here!")
			else:
				last_low = True
		elif c.isdigit():
			pass
		elif c == " ":
			last_low = False
		else:
			raise ParserError(self, "Found wrong character '%c'," % c + 
				" name must contain only alphabetic characters, "\
				"numbers and whitespaces!")
	return name


class Iterator:
	def __init__(self, list, reference):
		self.list = list
		self.list.sort()
		self.reference = reference
	
	def __iter__(self):
		return self
	
	def next(self):
		try:
			while True:
				item = self.list.pop(0)
				if self.reference != None and item.reference != self.reference:
					continue
				return item
		except IndexError:
			raise StopIteration()

class SortedDict(dict):
	"""
	A dictionary with an Iterator how sorts the output
	"""
	def __iter__(self):
		return self.iter()
	
	def iter(self, reference=False):
		"""
		Generate an iterator.
		
		Keywords arguments:
		reference	--	None: iterate over all elements
						True: only over the referenced elements
						False (default): only over the not referenced elements
		"""
		return Iterator(self.values(), reference)

class SingleAssignDict(SortedDict):
	"""
	A dictionary which don't allow overwritting attributes after
	the initial creation.
	"""
	def __init__(self,  name):
		self.name = name
		SortedDict.__init__(self)
	
	def __setitem__(self, key, item):
		if not self.has_key(key):
			SortedDict.__setitem__(self, key, item)
		else:
			raise ParserError("%s \"%s\" defined twice! Check the xml-file!" % (self.name.capitalize(),  key))
	
	def remove(self, key):
		SortedDict.pop(self, key)
	
	def replace(self, key, item):
		SortedDict.__setitem__(self, key, item)

#!/usr/bin/env python
# -*- coding: utf-8 -*-

from exception import ParserError
import helper


class SubType:
	""" Subordinate type for struct and typedefs.
	
	Used to distinguish between normal from array types.
	
	Attributes:
	raw		--	raw typename.
	name	--	type without a possible array definition.
	count	--	Number of array elements. Defaults to 1 if the type is no
				array.
	
	Example:
	type = SubType("uint8_t")
	=> .raw = "uint8_t"
	   .name = "uint8_t"
	   .is_array = False
	   .count = 1
	
	type = SubType("char[8])
	=> .raw = "char[8]"
	   .name = "char"
	   .is_array = True
	   .count = 8
	
	"""
	def __init__(self, value):
		self.raw = value
		if value.endswith(']'):
			self.is_array = True
			self.name, number = value.split('[')
			self.count = number[:-1]
		else:
			self.is_array = False
			self.count = 1
			self.name = value
	
	def __str__(self):
		return self.raw


class BaseType(object):
	""" Abstract base class for all types
	
	Arguments:
	name		--	Type name. Must be unique.
	reference	--	True if the type is only used as reference for other
					types and should not be included in the output.
	desc		--	Description of the type
	is_enum		--	True if the type is an enum type
	is_struct	--	analog
	is_typedef	--	analog
	_checked	--	Flag that indicates that the level is set correctly
	level		--	hierarchy level. Built-in types have level -1, the level
					of a type is the highest level of a subtype plus one.
	"""
	def __init__(self, name, reference=False):
		self.name = self._check_name(name)
		self.reference = reference
		
		self.desc = None
		self.is_enum = False
		self.is_struct = False
		self.is_typedef = False
		
		# used to determine the hierachy level later
		self._checked = False
		self.level = -1
	
	def check(self):
		""" Check if the hierachy level could be determined. """
		if not self._checked:
			raise ParserError("Error creating the type hierarchy (check %s)" % self.name)
	
	def _from_xml(self, node):
		""" Load the state of the class from a xml tree. """
		pass
	
	def _check_name(self, name):
		""" Check if the name for the type conform to certain rules """
		# TODO replace this with a regular expression
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
	
	def __cmp__(self, other):
		"""
		If types are sorted they are sorted first by level and then
		by name.
		"""
		return cmp(self.level, other.level) or cmp(self.name, other.name)


class BuiltIn(BaseType):
	""" Built-in types are available by default. """
	def __init__(self, name):
		# the build-in types are always only for reference
		BaseType.__init__(self, name, True)
		
		self._checked = True
		self.level = -1
	
	def _check_name(self, name):
		return name
	
	def __str__(self):
		return "%s (built-in)" % self.name


class Enum(BaseType):
	
	class Element(object):
		""" Sub-element of an enum-type
		
		The name of the element has to be all upper case with underscores
		"""
		def __init__(self, name, desc=None, value=None): 
			self.name = self._check_name(name)
			self.desc = desc
			self.value = value
		
		def _from_xml(self, node):
			self.desc = helper.xml_read_description(node)
		
		def _check_name(self, name):
			for c in name:
				if not ((c.isalpha() and c.isupper()) or c.isdigit() or c == "_"):
					raise ParserError(self, "error in name notation")
			return name
		
		def __str__(self):
			return "%s = %s" % (self.name, self.value)
	
	def __init__(self, name, reference=False):
		BaseType.__init__(self, name, reference)
		
		self.last_value = 0
		self._elements = []
		
		self.is_enum = True
		
		# an enum don't depend on other types
		self._checked = True
		self.level = 0
	
	def add(self, element):
		""" Add an element to the enum.
		
		This has to be done in the order the elements should apear later.
		"""
		if element.value == None or element.value == '':
			element.value = self.last_value
			self.last_value += 1
		else:
			try:
				element.value = int(str(element.value), 0)
				self.last_value = element.value + 1
			except ValueError:
				pass
		
		self._elements.append(element)
	
	def iter(self):
		""" Iterate over all sub-elements of the enum """
		for element in self._elements:
			yield element
	
	def _from_xml(self, node):
		self.desc = helper.xml_read_description(node)
		
		for elem in node.findall('element'):
			value = elem.get('value')
			if value == None:
				value = elem.text
				if value is not None:
					value = value.strip(' \t\n')
			
			element = self.Element(	name = elem.get('name'), 
									value = value)
			element._from_xml(elem)
			self.add(element)
	
	def __str__(self):
		ref = "R" if (self.reference) else ""
		str = "%s (enum, %i%s)\n" % (self.name,  self.level, ref)
		for element in self.iter():
			str += "+ %s\n" % element
		return str[:-1]


class Struct(BaseType):
	
	class Element(object):
		
		def __init__(self, name):
			self.name = name
			
			self.desc = None
			self.type = None
			self.unit = None
			
			# used to determine the hierachy level later
			self._checked = False
			self.level = -1
		
		def _from_xml(self,  node):
			self.desc = helper.xml_read_description(node)
			self.type = SubType(node.get('type'))
			self.unit = node.get('unit')
		
		def __str__(self):
			return "%s : %s" % (self.name, self.type)
	
	def __init__(self, name, reference=False):
		BaseType.__init__(self, name, reference)
		
		self.is_struct = True
		self.elements = []
	
	def add(self, element):
		self.elements.append(element)
	
	def iter(self):
		""" Iterate over all sub-elements of the enum """
		for element in self.elements:
			yield element
	
	def _from_xml(self, node):
		self.desc = helper.xml_read_description(node)
		for elem in node.findall('element'):
			element = self.Element(elem.get('name'))
			element._from_xml(elem)
			
			self.add(element)
	
	def __str__(self):
		ref = "R" if (self.reference) else ""
		str = "%s (struct, %i%s)\n" % (self.name, self.level, ref)
		for element in self.iter():
			str += "+ %s\n" % element
		return str[:-1]


class Typedef(BaseType):
	
	def __init__(self, name, reference=False):
		BaseType.__init__(self, name, reference)
		
		self.type = None
		self.unit = None
		
		self.is_typedef = True
	
	def _from_xml(self,  node):
		self.desc = helper.xml_read_description(node)
		self.type = SubType(node.get('type'))
		self.unit = node.get('unit')
	
	def __str__(self):
		ref = "R" if (self.reference) else ""
		return "%s (typedef, %i%s)\n=> %s" % (self.name, self.level, ref, self.type)

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	enum = Enum("test", "eine Beschreibung")
	enum.add(EnumElement("test2", value=2))
	enum.add(EnumElement("test3"))
	enum.add(EnumElement("test4", value="0x12"))
	enum.add(EnumElement("test5"))
	
	print enum

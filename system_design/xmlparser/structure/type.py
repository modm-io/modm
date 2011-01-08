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
	isArray	--	Subtype is Arraytype
	
	Example:
	type = SubType("uint8_t")
	=> .raw = "uint8_t"
	   .name = "uint8_t"
	   .isArray = False
	   .count = 1
	
	type = SubType("char[8])
	=> .raw = "char[8]"
	   .name = "char"
	   .isArray = True
	   .count = 8
	
	"""
	def __init__(self, value):
		self.raw = value
		if value.endswith(']'):
			self.isArray = True
			self.name, number = value.split('[')
			self.count = number[:-1]
		else:
			self.isArray = False
			self.count = 1
			self.name = value
		self.size = None
	
	def __str__(self):
		return self.raw


class BaseType(object):
	""" Abstract base class for all types
	
	Arguments:
	name		--	Type name. Must be unique.
	reference	--	True if the type is only used as reference for other
					types and should not be included in the output.
	description		--	Description of the type
	isEnum		--	True if the type is an enum type
	isStruct	--	analog
	isTypedef	--	analog
	_checked	--	Flag that indicates that the level is set correctly
	level		--	hierarchy level. Built-in types have level -1, the level
					of a type is the highest level of a subtype plus one.
	size		--	Size
	"""
	def __init__(self, name, reference=False):
		self.name = self._check_name(name)
		self.reference = reference
		
		self.description = None
		self.isEnum = False
		self.isStruct = False
		self.isTypedef = False
		
		# used to determine the hierachy level later
		self._checked = False
		self.level = -1
		self.size = None
	
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
						raise ParserError(self, "Don't use CamelCase here! (%s)" % name)
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
	""" Built-in types, read from xml. """
	def __init__(self, name, reference=False):
		# the build-in types are always only for reference
		BaseType.__init__(self, name, reference=True)
		
		self._checked = True
		self.level = -1
	
	def _from_xml(self,  node):
		self.description = helper.xml_read_description(node)
		self.size = int(node.get('size'))
	
	def _check_name(self, name):
		return name
	
	def __str__(self):
		ref = "R" if (self.reference) else ""
		return "%s (built-in|%i, %i%s)" % \
			(self.name, self.size, self.level, ref)

class Enum(BaseType):
	
	class Element(object):
		""" Sub-element of an enum-type
		
		The name of the element has to be all upper case with underscores
		"""
		def __init__(self, name): 
			self.name = self._check_name(name)
			self.description = None
			self.value = None
		
		def _from_xml(self, node):
			self.description = helper.xml_read_description(node)
			self.value = helper.xml_read_enum_element_value(node);
		
		def _check_name(self, name):
			for c in name:
				if not ((c.isalpha() and c.isupper()) or c.isdigit() or c == "_"):
					raise ParserError(self, "Attribute name of element in enum has to be UPPER_UNDERSCORE_STYLE (found: '%s')" % name)
			return name
		
		def __str__(self):
			return "%s = %s" % (self.name, self.value)
	
	def __init__(self, name, reference=False):
		BaseType.__init__(self, name, reference)
		
		self.last_value = 0
		self._elements = []
		
		self.isEnum = True
		
		# an enum don't depend on other types
		self._checked = True
		self.level = 0
		self.size = 1		# FIXME calculate actual size depending on the value for the enum elements
	
	def add(self, element):
		""" Add an element to the enum.
		
		This has to be done in the order the elements should apear later.
		"""
		if element.value == None:
			element.value = self.last_value
			self.last_value += 1
		else:
			try:
				self.last_value = element.value + 1
			except ValueError:
				pass
		
		self._elements.append(element)
	
	def iter(self):
		""" Iterate over all sub-elements of the enum """
		for element in self._elements:
			yield element
	
	def _from_xml(self, node):
		self.description = helper.xml_read_description(node)
		
		for elem in node.findall('element'):
			element = self.Element(name = elem.get('name'))
			element._from_xml(elem)
			self.add(element)
	
	def __str__(self):
		ref = "R" if (self.reference) else ""
		str = "%s (enum|%i, %i%s)\n" % \
			(self.name, self.size, self.level, ref)
		for element in self.iter():
			str += "+ %s\n" % element
		return str[:-1]


class Struct(BaseType):
	
	class Element(object):
		
		def __init__(self, name):
			self.name = name
			
			self.description = None
			self.type = None
			self.unit = None
			
			# used to determine the hierachy level later
			self._checked = False
			self.level = -1
		
		def _from_xml(self,  node):
			self.description = helper.xml_read_description(node)
			self.type = SubType(node.get('type'))
			self.unit = node.get('unit')
		
		def __str__(self):
			return "%s : %s" % (self.name, self.type)
	
	def __init__(self, name, reference=False):
		BaseType.__init__(self, name, reference)
		
		self.isStruct = True
		self.extends = None
		self.elements = []
	
	def add(self, element):
		self.elements.append(element)
	
	def extend(self, struct):
		self.extends = struct.extends
		i = 0
		for e in struct.elements:
			self.elements.insert(i, e)
			i = i+1
	
	def iter(self):
		""" Iterate over all sub-elements of the enum """
		for element in self.elements:
			yield element
	
	def _from_xml(self, node):
		self.description = helper.xml_read_description(node)
		self.extends = node.get('extends')
		for elem in node.findall('element'):
			element = self.Element(elem.get('name'))
			element._from_xml(elem)
			
			self.add(element)
	
	def __str__(self):
		ref = "R" if (self.reference) else ""
		str = "%s (struct|%i, %i%s)\n" % \
			(self.name, self.size, self.level, ref)
		for element in self.iter():
			str += "+ %s\n" % element
		return str[:-1]


class Typedef(BaseType):
	
	def __init__(self, name, reference=False):
		BaseType.__init__(self, name, reference)
		
		self.type = None
		self.unit = None
		
		self.isTypedef = True
	
	def _from_xml(self,  node):
		self.description = helper.xml_read_description(node)
		self.type = SubType(node.get('type'))
		self.unit = node.get('unit')
	
	def __str__(self):
		ref = "R" if (self.reference) else ""
		return "%s (typedef|%i, %i%s)\n=> %s" % \
			(self.name, self.size, self.level, ref, self.type)

# -----------------------------------------------------------------------------
if __name__ == '__main__':
	enum = Enum("test", "eine Beschreibung")
	enum.add(Enum.Element("test2", value=2))
	enum.add(Enum.Element("test3"))
	enum.add(Enum.Element("test4", value="0x12"))
	enum.add(Enum.Element("test5"))
	
	print enum

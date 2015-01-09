#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import copy

from parser_exception import ParserException
import utils
import xml_utils

class BaseType(object):
	""" Abstract base class for all types
	
	Attributes:
	name		--	Type name. Must be unique.
	reference	--	True if the type is only used as reference for other
					types and should not be included in the output.
	description	--	Description of the type
	string		--  Short description for enum members
	isEnum		--	True if the type is an enum type
	isStruct	--	analog
	isTypedef	--	analog
	isBuiltIn	--	analog
	level		--	hierarchy level. Built-in types have level -1, the level
					of a type is the highest level of a subtype plus one.
	size		--	Size
	"""
	def __init__(self, node):
		self.node = node
		
		self.name = node.get('name')
		self._check_name()
		self.description = None
		self.string = None
		
		self.isEnum = False
		self.isEnumClass = False
		self.isStruct = False
		self.isTypedef = False
		self.isBuiltIn = False
		
		self.level = None
		self.size = None
	
	def _check_name(self):
		utils.check_name(self.name)
	
	def evaluate(self, tree):
		""" Load the state of the class from the corresponding XML node
		
		Keyword arguments:
		tree	--	current tree of the communcation structure. Must contain
					every available type, but some types may not be fully
					evaluated
		"""
		pass
	
	def create_hierarchy(self):
		"""	Create the type hierarchy
		
		This method calculates the values for self.size and self.level. Must
		not be called before all types are fully created.
		"""
		pass
	
	def flattened(self):
		""" Access the version with the flattened hierarchy """
		return self
	
	def __cmp__(self, other):
		""" Compare two types
		
		If types are sorted they are sorted first by level and then by name.
		"""
		if isinstance(other, BaseType):
			return cmp(self.level, other.level) or cmp(self.name, other.name)
		else:
			return 1


class BuiltIn(BaseType):
	""" Built-in types
	
	These types correspond directly to types available by default in the
	target environment. The hierarchy level will always be -1 (lowest level).
	"""
	def __init__(self, node):
		BaseType.__init__(self, node)
		
		self.isBuiltIn = True
		self.level = -1
	
	def evaluate(self, tree):
		if self.node is None:
			return
		
		self.description = xml_utils.get_description(self.node)
		self.string = xml_utils.get_string(self.node)
		self.size = int(self.node.get('size'))
		
		self.node = None
	
	def _check_name(self):
		""" Built-in types need no check """
		pass
	
	def dump(self):
		return "%s : built-in|%i [%i]" % (self.name, self.level, self.size)
	
	def __str__(self):
		return "%s : built-in" % self.name

class Enum(BaseType):
	
	class Element(object):
		""" Sub-element of an enum-type """
		def __init__(self, node):
			""" Constructor
			
			The name of the element has to be all upper case with underscores.
			"""
			self.name = node.get('name')
			if not re.match("^[0-9A-Z_]*$", self.name):
				raise ParserException("Attribute name of element in enum has to be UPPER_UNDERSCORE_STYLE (found: '%s')" % (self.name))
			
			self.string = node.get('string')
			if self.string is None:
				self.string = self.name
				
			self.description = xml_utils.get_description(node)
			self.string = xml_utils.get_string(node)
			
			value = node.get('value')
			self.value = None if (value is None) else int(value, 0)
		
		def __str__(self):
			return "%s = %s" % (self.name, self.value)
	
	def __init__(self, node):
		BaseType.__init__(self, node)
		
		self._last_value = 0
		self.elements = []
		
		self.isEnum = True
		
		# an enum don't depend on other types
		self.level = 0
		self.size = 1		# FIXME calculate actual size depending on the value for the enum elements
	
	def iter(self):
		""" Iterate over all sub-elements of the enum """
		for element in self.elements:
			yield element
	
	def evaluate(self, tree):
		if self.node is None:
			return
		
		self.description = xml_utils.get_description(self.node)
		self.string = xml_utils.get_string(self.node)
		for node in self.node.findall('element'):
			self.__add(self.Element(node))
		
		self.node = None
	
	def __add(self, element):
		""" Add an element to the enum.
		
		This has to be done in the order the elements should appear later.
		"""
		if element.value == None:
			element.value = self._last_value
			self._last_value += 1
		else:
			try:
				self._last_value = element.value + 1
			except ValueError:
				pass
		
		self.elements.append(element)
	
	def create_hierarchy(self):
		pass
	
	def dump(self):
		str = "%s : enum|%i [%i]\n" % (self.name, self.level, self.size)
		for element in self.iter():
			str += "  + %s\n" % element
		return str[:-1]
	
	def __str__(self):
		return "%s : enum|%i" % (self.name, self.level)


class EnumClass(BaseType):

	class Element(object):
		""" Sub-element of an enum-type """
		def __init__(self, node):
			""" Constructor

			The name of the element has to be all upper case with underscores.
			"""
			self.name = node.get('name')
			# if not re.match("^[0-9A-Z_]*$", self.name):
			# 	raise ParserException("Attribute name of element in enum has to be UPPER_UNDERSCORE_STYLE (found: '%s')" % (self.name))

			self.string = node.get('string')
			if self.string is None:
				self.string = self.name

			self.description = xml_utils.get_description(node)
			self.string = xml_utils.get_string(node)

			value = node.get('value')
			self.value = None if (value is None) else int(value, 0)

		def __str__(self):
			return "%s = %s" % (self.name, self.value)

	def __init__(self, node):
		BaseType.__init__(self, node)

		self._last_value = 0
		self.elements = []

		self.isEnumClass = True

		# an enum don't depend on other types
		self.level = 0
		self.size = 1
		self.underlyingType = node.get('underlyingType')

	def iter(self):
		""" Iterate over all sub-elements of the enum """
		for element in self.elements:
			yield element

	def evaluate(self, tree):
		if self.node is None:
			return

		self.description = xml_utils.get_description(self.node)
		self.string = xml_utils.get_string(self.node)
		for node in self.node.findall('element'):
			self.__add(self.Element(node))

		self.node = None

	def __add(self, element):
		""" Add an element to the enum.

		This has to be done in the order the elements should appear later.
		"""
		if element.value == None:
			element.value = self._last_value
			self._last_value += 1
		else:
			try:
				self._last_value = element.value + 1
			except ValueError:
				pass

		self.elements.append(element)

	def create_hierarchy(self):
		pass

	def dump(self):
		str = "%s : enum class|%i [%i]\n" % (self.name, self.level, self.size)
		for element in self.iter():
			str += "  + %s\n" % element
		return str[:-1]

	def __str__(self):
		return "%s : enum class|%i" % (self.name, self.level)


class SubType:
	""" Subordinate type for struct and typedefs.
	
	Used to distinguish between normal and array types for the subtype of
	struct elements and typedefs.
	
	Attributes:
	raw		--	raw typename.
	name	--	type without a possible array definition.
	count	--	Number of array elements. Defaults to 1 if the type is no array.
	isArray	--	Subtype is an array
	
	Example:
	type = SubType("uint8_t")
	=> .raw = "uint8_t"
	   .name = "uint8_t"
	   .isArray = False
	   .count = 1
	   .type = BuiltIn("uint8_t")
	
	type = SubType("char[8])
	=> .raw = "char[8]"
	   .name = "char"
	   .isArray = True
	   .count = 8
	   .type = BuiltIn("char")
	
	"""
	def __init__(self, value, types):
		""" Constructor
		
		Keyword Arguments:
		value	-- type name
		types	-- list of all available types
		"""
		self.raw = value
		if value.endswith(']'):
			self.isArray = True
			self.name, number = value.split('[')
			self.count = number[:-1]
		else:
			self.isArray = False
			self.count = 1
			self.name = value
		
		try:
			self.type = types[self.name]
		except KeyError:
			raise ParserException("Unknown type '%s'" % self.name)
		self.size = None
	
	def __str__(self):
		return self.raw


class Struct(BaseType):
	""" Representation of a Struct
	
	Attributes:
	size	--	Size of the struct in bytes. Will always return the total size
				of the flattened struct if the struct is part of inheritence
				structure.
	"""
	class Element(object):
		
		def __init__(self, node, tree):
			self.name = node.get('name')
			
			self.description = xml_utils.get_description(node)
			self.string = xml_utils.get_string(node)
			self.subtype = SubType(node.get('type'), tree.types)
			self.unit = node.get('unit')
			self.value = node.get('value')
			
			self.level = None
			self.size = None
		
		def create_hierarchy(self):
			subtype = self.subtype.type
			if subtype.level is None:
				subtype.create_hierarchy()
			
			self.level = subtype.level
			self.size = subtype.size
		
		def __str__(self):
			return "%s : %s" % (self.name, self.subtype)
	
	def __init__(self, node):
		BaseType.__init__(self, node)
		
		self.isStruct = True
		self.elements = []
		self.extends = None
		self.extending = []
		self.typeIdentifier = None
		
		self.__flattened = None
		self.__typeIdentifierName = None
	
	def iter(self):
		""" Iterate over all sub-elements of the enum """
		for element in self.elements:
			yield element
	
	def evaluate(self, tree):
		if self.node is None:
			return
		
		self.description = xml_utils.get_description(self.node)
		self.string = xml_utils.get_string(self.node)
		for node in self.node.findall('element'):
			try:
				self.elements.append(self.Element(node, tree))
			except ParserException as e:
				raise ParserException("Error in definition of struct '%s': %s!" % (self.name, e))
		
		basetype = self.node.get('extends')
		if basetype is not None:
			try:
				self.extends = tree.types[basetype]
				if not self.extends.isStruct:
					raise ParserException("Struct '%s' is derived from non struct '%s'!" % (self.name, self.extends.name))
				if self.extends.extends:
					raise ParserException("Struct '%s' extends struct '%s'. Structs are only allowed to extend from those Structs, which do not extend anything!" % (self.name, self.extends.name))
				self.__typeIdentifierName = self.node.get('typeIdentifier')
				if self.__typeIdentifierName is None:
					raise ParserException("Struct '%s' does extend '%s', but does not provide the attribute 'typeIdentifier'!" % (self.name, self.extends.name))

			except KeyError:
				raise ParserException("Unknown super type '%s' in struct '%s'!" % (basetype, self.name))
		self.node = None
	
	def create_hierarchy(self):
		""" Create hierarchy
		
		For this method self.size = 0 is used as sepecial value to detect
		loops in the definition of types. In normal operation size will never be
		zero, only during hierarchy creation.
		"""
		if self.level is not None:
			return
		
		self.size = 0
		size = 0
		self.level = 0
		for element in self.elements:
			if element.size == 0:
				raise ParserException("Loop in the definition of '%s' and '%s' detected!" % (self.name, self.element.name))
			element.create_hierarchy()
			
			size += element.size
			self.level = max(self.level, element.level)
		
		if self.extends is not None:
			if self.extends.size == 0:
				raise ParserException("Loop in the definition of '%s' and '%s' detected!" % (self.name, self.extends.name))
			self.extends.create_hierarchy()
			typeIdentifierStructElement = self.extends.elements[0]
			if not typeIdentifierStructElement.subtype.type.isEnum:
				raise ParserException("Struct '%s' is extended by Struct '%s'. "  \
					"Structs which are extended by other must have an element named " \
					"'type' of any enum type as their first element! It is used for " \
					"type distinguishing at runtime." \
						% (self.extends.name, self.name))
			if not typeIdentifierStructElement.name == 'type':
				raise ParserException("Struct '%s' is extended by Struct '%s'. Structs" \
					"which are extended by other must have an element named 'type' as" \
					"their first element! It is used for type distinguishing at runtime." \
						% (self.extends.name, self.name))
			
			for enumElement in typeIdentifierStructElement.subtype.type.elements:
				if enumElement.name == self.__typeIdentifierName:
					self.typeIdentifier = enumElement
					break
			if not self.typeIdentifier:
				raise ParserException("Struct '%s' extends Struct '%s', but it's " \
					"typeIdentifier '%s' is not member of enum '%s' which is the " \
					"type of '%s.type'."
						% (self.name, self.extends.name, self.__typeIdentifierName,
							typeIdentifierStructElement.subtype.type.name, self.extends.name))
			
			self.extends.__addExtending(self)
			size += self.extends.size
			self.level = max(self.level, self.extends.level)
		
		if size > 48:
			raise ParserException("Struct '%s' is with %i Byte too big. The maximum " \
				"packet size is 48 Byte!" % (self.name, size))
		
		self.size = size
		self.level += 1
	
	def flattened(self):
		if self.__flattened is None:
			if self.extends is None:
				self.__flattened = self
			else:
				self.__flattened = copy.copy(self)
				self.__flattened.elements = self.elements[:]
				
				# prepend all elements for the super type
				self.__flattened.elements[0:0] = self.extends.flattened().elements
		
		return self.__flattened
	
	def dump(self):
		str = "%s : struct|%i [%i]\n" % \
			(self.name, self.level, self.size)
		for element in self.iter():
			str += "  + %s\n" % element
		return str[:-1]
	
	def __str__(self):
		return "%s : struct|%i" % (self.name, self.level)

	def __addExtending(self, extending):
		for struct in self.extending:
			if struct.typeIdentifier == extending.typeIdentifier:
				raise ParserException("Duplicate TypeIdentifier '%s' in Struct group extending '%s'. ('%s' and '%s')" % (extending.typeIdentifier.name, self.name, struct.name, extending.name))
		self.extending.append(extending)

class Typedef(BaseType):
	
	def __init__(self, node):
		BaseType.__init__(self, node)
		
		self.subtype = None
		self.unit = None
		
		self.isTypedef = True
	
	def evaluate(self, tree):
		self.description = xml_utils.get_description(self.node)
		self.string = xml_utils.get_string(self.node)
		self.unit = self.node.get('unit')
		try:
			self.subtype = SubType(self.node.get('type'), tree.types)
		except ParserException as e:
			raise ParserException("Error in definition of typedef '%s': %s" % (self.name, e))
	
	def create_hierarchy(self):
		""" Create hierarchy
		
		See Struct.create_hierarchy() for a detailed description of this
		method.
		"""
		if self.level is not None:
			return
		
		self.size = 0
		subtype = self.subtype.type
		if subtype.level is None:
			if self.subtype.size == 0:
				raise ParserException("Loop in the definition of '%s' and '%s' detected!" % (self.name, self.subtype.name))
			subtype.create_hierarchy()
		
		self.level = subtype.level + 1
		self.size = subtype.size
	
	def dump(self):
		return "%s : typedef|%i [%i]\n  -> %s" % (self.name, self.level, self.size, self.subtype)
	
	def __str__(self):
		return "%s : typedef|%i" % (self.name, self.level)

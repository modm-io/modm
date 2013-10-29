# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All Rights Reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.

class Parameter:
	""" Parameter
	Represents a parameter ...
	"""

	def __init__(self, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		# initialize all members to None
		self.driver_type = None
		self.driver_name = None
		self.driver_instance = None
		self.name = None

	@classmethod
	def fromUserConfigString(self, user_string, logger=None):
		"""fromUserConfigString
		"""
		incorrect_string_msg = 	"Incorrect parameter config line!\n"
								"Valid inputs are:\n"
								"driver_type.parameter_name = value\n"
								"driver_type.driver_instance.parameter_name = value\n"
								"driver_type.driver_name.parameter_name = value\n"
								"driver_type.driver_name.driver_instance.parameter_name = value"
		# Create Parameter
		p = Parameter(logger)
		# Sanity check user input
		if not '=' in user_string:
			p.log.error(incorrect_string_msg)
			return None
		[lvalue, rvalue] = user_string.split('=')
		parts = l_value.split('.')
		if len(parts) not in [2,3,4]:
			p.log.error(incorrect_string_msg)
			return None
		p.driver_type = parts[0]
		p.name = parts[-1:]
		if len(parts) == 3:
			if parts[1].isdigit():
				p.driver_instance = int(parts[1])
			else:
				p.driver_name = parts[1]
		elif len(parts) == 4:
			p.driver_name = parts[1]
			p.driver_instance = int(parts[2])
		return p

	@classmethod
	def fromUserConfigString(self, user_string, logger=None):
		"""fromUserConfigString
		"""
		incorrect_string_msg = 	"Incorrect parameter config line!\n"
								"Valid inputs are:\n"
								"driver_type.parameter_name = value\n"
								"driver_type.driver_instance.parameter_name = value\n"
								"driver_type.driver_name.parameter_name = value\n"
								"driver_type.driver_name.driver_instance.parameter_name = value"
		# Create Parameter
		p = Parameter(logger)




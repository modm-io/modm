# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All Rights Reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.


class ParameterDB:
	""" Parameter Data Base
	Manages Parameters
	"""

	def __init__(self, userParameters=None, logger=None):
		if logger == None:
			self.log = Logger()
		else:
			self.log = logger
		self._parameters = []
		self.addUserParameters(userParameters)

	def addUserParameters(self, items):
		"""
		Parses and adds items form [parameters] section in `project.cfg`
		"""
		if items is None:
			return
		for item in items:
			p = Parameter.fromUserConfigItem(item, self.log)
			if p is not None:
				self._parameters.append(p)

	def addDriverParameter(self, param):
		"""
		"""
		self.log.error("Please implement ParameterDB.addDriverParameter")

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
		self.value = None

	@classmethod
	def fromUserConfigItem(self, user_item, logger=None):
		"""fromUserConfigString
		"""
		incorrect_string_msg = 	("Incorrect parameter config line!\n"
								 "Valid inputs are:\n"
								 "driver_type.parameter_name = value\n"
								 "driver_type.driver_instance.parameter_name = value\n"
								 "driver_type.driver_name.parameter_name = value\n"
								 "driver_type.driver_name.driver_instance.parameter_name = value")
		# Create Parameter
		p = Parameter(logger)
		# Sanity check user input
		parts = user_item[0].split('.')
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
		p.value = user_item[1]
		return p




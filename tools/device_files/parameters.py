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
			p = UserParameter.fromUserConfigItem(item, self.log)
			if p is not None:
				self._parameters.append(p)

	def addDriverParameter(self, param):
		"""
		"""
		self.log.error("Please implement ParameterDB.addDriverParameter")

	def getParametersForDriver(self, driver):
		parameters = []
		for param in self._parameters:
			if param.driver_type == driver.type and param.driver_name == driver.name:
				parameters.append({'name': param.name, 'value': param.value, 'instance': param.driver_instance})
		return parameters

	@classmethod
	def fromDictionary(self, dictionary, logger=None):
		"""fromDictionary
		"""
		p = ParameterDB(userParameters=None, logger=logger)
		for param in dictionary['_parameters']:
			p._parameters.append(UserParameter.fromDictionary(param, p.log))
		return p


	def toDictionary(self):
		""" Serializes this object into a dictionary that can be used for
			dependency tracking with scons.
		"""
		param_dicts = []
		for p in self._parameters:
			param_dicts.append(p.toDictionary())
		return { '_parameters': param_dicts }

class UserParameter:
	""" UserParameter
	Represents a parameter declared by the user...
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
		self.level = None
		self.name = None
		self.value = None

	@classmethod
	def fromUserConfigItem(self, user_item, logger=None):
		"""fromUserConfigString
		"""
		incorrect_string_msg = 	("Incorrect parameter config line!\n"
								 "Valid inputs are:\n"
								 "\tdriver_type.parameter_name = value\n"
								 "\tdriver_type.driver_instance.parameter_name = value\n"
								 "\tdriver_type.driver_name.parameter_name = value\n"
								 "\tdriver_type.driver_name.driver_instance.parameter_name = value")
		# Create Parameter
		p = UserParameter(logger)
		# Sanity check user input
		parts = user_item[0].split('.')
		if len(parts) not in [2,3,4]:
			p.log.error(incorrect_string_msg)
			return None
		p.driver_type = parts[0]
		p.name = parts[-1:][0]
		if len(parts) == 3:
			if parts[1].isdigit():
				p.driver_instance = int(parts[1])
			else:
				p.driver_name = parts[1]
		elif len(parts) == 4:
			p.driver_name = parts[1]
			p.driver_instance = int(parts[2])
		p.value = int(user_item[1])
		p.level = 'user' # this parameter comes directly from the user
		return p

	@classmethod
	def fromDictionary(self, dictionary, logger=None):
		"""fromDictionary
		"""
		p = UserParameter(logger)
		p.driver_type     = dictionary['driver_type']
		p.driver_name     = dictionary['driver_name']
		p.driver_instance = dictionary['driver_instance']
		p.level           = dictionary['level']
		p.name            = dictionary['name']
		p.value           = dictionary['value']
		return p

	def toDictionary(self):
		""" Serializes this object into a dictionary that can be used for
			dependency tracking with scons.
		"""
		return {'driver_type':     self.driver_type,
				'driver_name':     self.driver_name,
				'driver_instance': self.driver_instance,
				'level':           self.level,
				'name':            self.name,
				'value':           self.value}




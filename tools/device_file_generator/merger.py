# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

import sys, os
# add python module logger to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'logger'))
from logger import Logger

class DeviceMerger:
	""" DeviceMerger
	Merges several devices into logical groups.
	Since most Devices have the same peripherals, but differ in other
	parameters, like Flash/RAM/EEPROM sizes, packaging or other minor
	differences, it makes sense to group them accordingly.

	Please not that this class only makes the resulting XML files more user
	friendly to manually edit, since the user does not have to apply the
	changes to multiple files. Ideally - reality might differ :(.
	"""

	def __init__(self, devices, logger=None):
		self.mergedDevices = list(devices)

		if logger == None:
			self.log = Logger()
		else:
			self.log = logger

	def mergedByPlatform(self, platform):
		if platform == 'avr':
			self.mergedByType()
			self.mergedByName()
		elif platform == 'stm32':
			self.mergedBySize()
			self.mergedByName()

	def mergedByType(self):
		self.mergedDevices = self._mergeDevicesByType(self.mergedDevices)

	def mergedByName(self):
		self.mergedDevices = self._mergeDevicesByName(self.mergedDevices)

	def mergedBySize(self):
		self.mergedDevices = self._mergeDevicesBySize(self.mergedDevices)


	def _mergeDevicesByName(self, devices):
		"""
		This is a simple helper method to merge devices based on name.
		"""
		avrDevices = []
		xmegaDevices = []
		stm32Devices = []
		result = []

		for dev in devices:
			if dev.ids.intersection.platform == 'avr':
				if dev.ids.intersection.family == 'xmega':
					xmegaDevices.append(dev)
				else:
					avrDevices.append(dev)
			elif dev.ids.intersection.platform == 'stm32':
				stm32Devices.append(dev)
			else:
				result.append(dev)

		avrDevices = self._mergeDevicesByNameAVR(avrDevices)
		xmegaDevices = self._mergeDevicesByNameXMEGA(xmegaDevices)
		stm32Devices = self._mergeDevicesByNameSTM32(stm32Devices)

		result.extend(avrDevices)
		result.extend(xmegaDevices)
		result.extend(stm32Devices)

		return result

	def _mergeDevicesByNameSTM32(self, devices):
		"""
		This checks the size-id and name of the devices, and merges the devices
		based on the observation, that the size-id only influences the size of
		memories, i.e. FLASH, RAM.
		"""
		# copy the devices, since this array will be modified
		devs = list(devices)
		merged = []

		while len(devs) > 0:
			current = devs[0]
			devs.remove(current)

			matches = []

			name_ids = self._getCategoryNameSTM32(current)
			size_ids = self._getCategorySizeSTM32(current)
			self.log.info("ByName: Searching for device with names '%s' and size-ids '%s'" % (name_ids, size_ids))

			for dev in devs:
				if dev.ids.getAttribute('name')[0] in name_ids and \
						dev.ids.getAttribute('size_id')[0] in size_ids:
					matches.append(dev)

			for match in matches:
				devs.remove(match)
				current = current.getMergedDevice(match)

			if len(matches) == 0:
				self.log.info("ByName: no match for device: " + current.id.string)

			self.log.debug("ByName:\nResulting device:\n" + str(current))
			merged.append(current)

		return merged

	def _getCategoryNameSTM32(self, device):
		names = device.ids.getAttribute('name')
		family = device.id.family

		if family == 'f0':
			categories = [	['050', '051'],
							['030']  ]
		elif family == 'f1':
			categories = [	['100', '102', '101', '103'],
							['105', '107']  ]
		elif family == 'f2':
			categories = [	['205', '207', '215', '217']  ]
		elif family == 'f3':
			categories = [	['301'],
							['302'],
							['303'],
			                ['334'],
			                ['318', '328', '358', '378'],
							['373']  ]
		elif family == 'f4':
			categories = [	['401'],
							['405', '415', '407', '417'],
							['427', '437', '429', '439']  ]

		# make sure that only one category is used!
		for cat in categories:
			if names[0] in cat:
				return cat

		return categories[0]

	def _mergeDevicesByNameXMEGA(self, devices):
		"""
		This checks the size-id and name of the devices, and merges the devices
		based on the observation, that the size-id only influences the size of
		memories, i.e. FLASH, RAM, and EEPROM.
		"""
		# copy the devices, since this array will be modified
		devs = list(devices)
		merged = []

		while len(devs) > 0:
			current = devs[0]
			devs.remove(current)

			matches = []
			type = current.ids.getAttribute('type')[0]

			if type != None:
				pin_id = current.ids.getAttribute('pin_id')[0]

				self.log.info("ByName: Searching for device with type '%s'" % type)

				for dev in devs:
					if dev.ids.getAttribute('type')[0] == type:
						# A3 none|b and bu|u are different enough to warrant
						# a new device file
						if type == 'a3':
							if dev.ids.getAttribute('pin_id')[0] in self._getCategoryPinIdAVR(current):
								matches.append(dev)
						else:
							matches.append(dev)

			for match in matches:
				devs.remove(match)
				current = current.getMergedDevice(match)

			if len(matches) == 0:
				self.log.info("ByName: no match for device: " + current.id.string)

			self.log.debug("ByName:\nResulting device:\n" + str(current))
			merged.append(current)

		return merged

	def _mergeDevicesByNameAVR(self, devices):
		"""
		This checks the size-id and name of the devices, and merges the devices
		based on the observation, that the size-id only influences the size of
		memories, i.e. FLASH, RAM, and EEPROM.
		"""
		# copy the devices, since this array will be modified
		devs = list(devices)
		merged = []

		while len(devs) > 0:
			current = devs[0]
			devs.remove(current)

			matches = []
			size_id = current.ids.getAttribute('size_id')[0]

			if size_id != None:
				name = current.ids.getAttribute('name')[0]
				type = current.ids.getAttribute('type')[0]
				family = name[len(size_id):]

				if not (family == "" and type == None):
					type = self._getCategoryTypeAVR(current)

					self.log.info("ByName: Searching for device ending in '" + family + "' and '" + str(type) + "'")

					for dev in devs:
						dname = dev.ids.getAttribute('name')[0]
						dsize_id = dev.ids.getAttribute('size_id')[0]

						# if they do not have a size-id they are probably unmergable
						if dsize_id != None:
							dfamily = dname[len(dsize_id):]

							# perpare for type comparison
							# we should only merge when the family is the same,
							# and if the type is the same

							if dfamily == family and dev.ids.getAttribute('type')[0] in type:
								matches.append(dev)

			# The following code is Atmel's fault with their stupid naming schemes.
			# the AT90's, ATmega's and ATtiny's have special merging rules
			if current.id.family == "at90":
				name = current.id.name

				# Some Devices are just not in the same group
				if name in ['1', '2', '3', '216', '316', '646', '647', '1286', '1287']:
					# these are not the matches you are looking for *move hand*
					matches = []
				# these are not the devices you want to matched with
				for match in matches:
					if match.id.name in ['1', '2', '3', '216', '316', '646', '647', '1286', '1287']:
						matches.remove(match)
						break
				# but these are:
				namesA = [ ['1', '2', '216'], ['3', '316'], ['646', '647', '1286', '1287'] ]
				for names in namesA:
					if name in names:
						for dev in [d for d in devs if dev.id.family == "at90"]:
							for dname in dev.ids.getAttribute('name'):
								if dname in names:
									matches.append(dev)

			if current.id.family == "atmega":
				name = current.id.name

				if current.ids.getAttribute('type')[0] in [None, 'none', 'p', 'a', 'pa']:
					# Some Devices are just not in the same group
					if name in ['8', '16', '32', '64', '128']:
						# these are not the matches you are looking for *move hand*
						matches = []
					# these are not the devices you want to be matched with
					for match in matches:
						if match.id.name in ['8', '16', '32', '64', '128']:
							matches.remove(match)
							break
					# but these are:
					namesA = [ ['16', '32'], ['64', '128'] ]
					for names in namesA:
						if name in names:
							for dev in devs:
								if dev.id.family == "atmega" and dev.ids.getAttribute('type')[0] in [None, 'none', 'p', 'a', 'pa']:
									for dname in dev.ids.getAttribute('name'):
										if dname in names:
											matches.append(dev)

			if current.id.family == "attiny":
				name = current.id.name
				names = ['4', '5', '9', '10']
				if name in names:
					for dev in devs:
						if dev.id.family == "attiny":
							for dname in dev.ids.getAttribute('name'):
								if dname in names:
									matches.append(dev)

				# Some Devices are just not in the same group
				if name in ['28', '20', '40']:
					# these are not the matches you are looking for *move hand*
					matches = []
				# these are not the devices you want to matched with
				for match in matches:
					if match.id.name in ['28', '20', '40']:
						matches.remove(match)
						break

			for match in matches:
				devs.remove(match)
				current = current.getMergedDevice(match)

			if len(matches) == 0:
				self.log.info("ByName: no match for device: " + current.id.string)

			self.log.debug("ByName:\nResulting device:\n" + str(current))
			merged.append(current)

		return merged


	def _mergeDevicesBySize(self, devices):
		"""
		This is a simple helper method to merge devices based on size.
		"""
		stm32Devices = []
		result = []

		for dev in devices:
			if dev.id.platform == 'stm32':
				stm32Devices.append(dev)
			else:
				result.append(dev)

		stm32Devices = self._mergeDevicesBySizeSTM32(stm32Devices)
		result.extend(stm32Devices)

		return result

	def _mergeDevicesBySizeSTM32(self, devices):
		"""
		This checks the size-id and name of the devices, and merges the devices
		based on the observation, that the size-id only influences the size of
		memories, i.e. FLASH, RAM.
		"""
		# copy the devices, since this array will be modified
		devs = list(devices)
		merged = []

		while len(devs) > 0:
			current = devs[0]
			devs.remove(current)

			matches = []

			size_ids = self._getCategorySizeSTM32(current)
			name = current.ids.getAttribute('name')[0]
			pin_id = current.ids.getAttribute('pin_id')[0]
			self.log.info("BySize: Searching for device with size-id '%s'" % size_ids)

			for dev in devs:
				if dev.ids.getAttribute('name')[0] == name and \
					dev.ids.getAttribute('size_id')[0] in size_ids:
					matches.append(dev)

			matches.sort(key=lambda k : int(k.getProperty('pin-count').values[0].value), reverse=True)
			for match in matches:
				devs.remove(match)
				current = current.getMergedDevice(match)

			if len(matches) == 0:
				self.log.info("BySize: no match for device: " + current.id.string)

			self.log.debug("BySize:\nResulting device:\n" + str(current))
			merged.append(current)

		return merged

	def _getCategorySizeSTM32(self, device):
		size_ids = device.ids.getAttribute('size_id')
		family = device.id.family
		name = device.ids.getAttribute('name')[0]
		# these categories are dependent on name
		# these are the categories of mergeable size-ids
		if family == 'f0':
			categories = [	['4', '6', '8']  ]	# low and medium density
		elif family == 'f1':
			categories = [	['4', '6'],			# low density
							['8', 'b'],			# medium density
							['c', 'd', 'e'],	# high density
							['f', 'g']  ]		# super high density
			if name in ['105', '107']:
				categories = [	['8', 'b', 'c'] ]	# medium and high density
		elif family == 'f2':
			categories = [	['b', 'c', 'd', 'e', 'f', 'g'] ]	# high density
		elif family == 'f3':
			categories = [ ['4', '6', '8'], ['b', 'c', 'd', 'e'] ]
			if name in ['373']:
				categories = [['8', 'b', 'c']]
		elif family == 'f4':
			categories = [	['b', 'c', 'd'],
							['e', 'g', 'i']  ]
			if name in ['401']:
				categories = [	['b', 'c', 'd', 'e'] ]
			if name in ['411']:
					categories = [['c', 'e']]
		# make sure that only one category is used!
		for cat in categories:
			if size_ids[0] in cat:
				return cat

		return categories[0]

	def _mergeDevicesByType(self, devices):
		"""
		This is a simple helper method to merge devices based on type.
		"""
		avrDevices = []
		result = []

		for dev in devices:
			if dev.id.platform == 'avr' and dev.id.family != 'xmega':
				avrDevices.append(dev)
			else:
				result.append(dev)

		avrDevices = self._mergeDevicesByTypeAVR(avrDevices)
		result.extend(avrDevices)

		return result

	def _mergeDevicesByTypeAVR(self, devices):
		"""
		This checks the name suffix (for example 'P', 'A', 'PA') of the
		devices and merges them based on the observation, that the suffix
		does not have anything to do with the mapping of peripherals.
		"""
		devs = list(devices)
		merged = []

		while len(devs) > 0:
			current = devs[0]
			devs.remove(current)

			props = current.id
			if props.valid == False:
				continue

			matches = []
			suffix = self._getCategoryTypeAVR(current)

			self.log.info("ByType: Searching for device ending in " + str(suffix))

			for dev in devs:
				if dev.id.name == props.name and dev.id.type in suffix:
					matches.append(dev)

			for match in matches:
				devs.remove(match)
				current = current.getMergedDevice(match)

			if len(matches) == 0:
				self.log.info("ByType: No match for device: " + current.id.string)

			self.log.debug("ByType:\nResulting device:\n" + str(current))
			merged.append(current)

		return merged

	def _getCategoryPinIdAVR(self, device):
		type = device.ids.getAttribute('pin_id')[0]
		# these are the categories of mergable types
		categories = [	# Xmega devices
						[None, 'none', 'b'],
						['bu', 'u'],
						]
		# make sure that only one category is used!
		for cat in categories:
			if type in cat:
				return cat

		return categories[0]

	def _getCategoryTypeAVR(self, device):
		type = device.ids.getAttribute('type')[0]
		# these are the categories of mergable types
		categories = [ 	# ATmega devices
						[None, 'none', 'p', 'a', 'pa'],
						['rfa1', 'rfa2', 'rfr1', 'rfr2'],
						['hvb', 'hvbrevb'],
						['hve2'],
						['hva'],
						['u2'],
						['u4', 'u6'],
						['m1', 'c1'],
						# AT90 devices
						['can'],
						['pwm'],
						['usb'],
						]
		# make sure that only one category is used!
		for cat in categories:
			if type in cat:
				return cat

		return categories[0]

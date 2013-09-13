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
		

	def mergedByType(self):
		self.mergedDevices = self._mergeDevicesByType(self.mergedDevices)

	def mergedByName(self):
		self.mergedDevices = self._mergeDevicesByName(self.mergedDevices)

	
	def _mergeDevicesByName(self, devices):
		"""
		This is a simple helper method to merge devices based on name.
		"""
		avrDevices = []
		xmegaDevices = []
		stm32Devices = []
		result = []

		for dev in devices:
			if dev.id.platform == 'avr':
				if dev.id.family == 'xmega':
					xmegaDevices.append(dev)
				else:
					avrDevices.append(dev)
			if dev.id.platform == 'stm32':
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

			name = current.getDeviceAttributes('name')[0]
			self.log.info("ByName: Searching for device with name '%s'" % name)

			for dev in devs:
				if dev.getDeviceAttributes('name')[0] == name:
					matches.append(dev)

			matches.sort(key=lambda k : int(k.getAttributes('pin-count')[0]['value']), reverse=True)
			for match in matches:
#				print match.getAttributes('pin-count')
				devs.remove(match)
				current = current.getMergedDevice(match)

			if len(matches) == 0:
				self.log.info("ByName: no match for device: " + current.id.string)

			self.log.debug("ByName:\n\nResulting device: " + str(current))
			merged.append(current)

		return merged

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
			type = current.getDeviceAttributes('type')[0]

			if type != None:
				#name = current.getDeviceAttributes('name')[0]
				pin_id = current.getDeviceAttributes('pin_id')[0]

				self.log.info("ByName: Searching for device with type '%s'" % type)

				for dev in devs:
					if dev.getDeviceAttributes('type')[0] == type and dev.getDeviceAttributes('pin_id')[0] == pin_id:
						matches.append(dev)

			for match in matches:
				devs.remove(match)
				current = current.getMergedDevice(match)

			if len(matches) == 0:
				self.log.info("ByName: no match for device: " + current.id.string)

			self.log.debug("ByName:\n\nResulting device: " + str(current))
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
			size_id = current.getDeviceAttributes('size_id')[0]

			if size_id != None:
				name = current.getDeviceAttributes('name')[0]
				type = current.getDeviceAttributes('type')[0]
				family = name[len(size_id):]

				if not (family == "" and type == None):
					type = self._getCategoryTypeAVR(current)

					self.log.info("ByName: Searching for device ending in '" + family + "' and '" + str(type) + "'")

					for dev in devs:
						dname = dev.getDeviceAttributes('name')[0]
						dsize_id = dev.getDeviceAttributes('size_id')[0]

						# if they do not have a size-id they are probably unmergable
						if dsize_id != None:
							dfamily = dname[len(dsize_id):]

							# perpare for type comparison
							# we should only merge when the family is the same,
							# and if the type is the same

							if dfamily == family and dev.getDeviceAttributes('type')[0] in type:
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
							for dname in dev.getDeviceNames():
								if dname in names:
									matches.append(dev)

			if current.id.family == "atmega":
				name = current.id.name

				if current.getDeviceAttributes('type')[0] in [None, 'none', 'p', 'a', 'pa']:
					# Some Devices are just not in the same group
					if name in ['8', '16', '32', '64', '128']:
						# these are not the matches you are looking for *move hand*
						matches = []
					# these are not the devices you want to matched with
					for match in matches:
						if match.id.name in ['8', '16', '32', '64', '128']:
							matches.remove(match)
							break
					# but these are:
					namesA = [ ['16', '32'], ['64', '128'] ]
					for names in namesA:
						if name in names:
							for dev in devs:
								if dev.id.family == "atmega" and dev.getDeviceAttributes('type')[0] in [None, 'none', 'p', 'a', 'pa']:
									for dname in dev.getDeviceNames():
										if dname in names:
											matches.append(dev)

			if current.id.family == "attiny":
				name = current.id.name
				names = ['4', '5', '9', '10']
				if name in names:
					for dev in devs:
						if dev.id.family == "attiny":
							for dname in dev.getDeviceNames():
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

			self.log.debug("ByName:\n\nResulting device: " + str(current))
			merged.append(current)

		return merged


	def _mergeDevicesByType(self, devices):
		"""
		This is a simple helper method to merge devices based on type.
		"""
		avrDevices = []
		stm32Devices = []
		result = []

		for dev in devices:
			if dev.id.platform == 'avr' and dev.id.family != 'xmega':
				avrDevices.append(dev)
			elif dev.id.platform == 'stm32':
				stm32Devices.append(dev)
			else:
				result.append(dev)

		avrDevices = self._mergeDevicesByTypeAVR(avrDevices)
		stm32Devices = self._mergeDevicesByTypeSTM32(stm32Devices)
		result.extend(avrDevices)
		result.extend(stm32Devices)

		return result

	def _mergeDevicesByTypeSTM32(self, devices):
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

			pin_id = current.getDeviceAttributes('pin_id')[0]
			name = current.getDeviceAttributes('name')[0]
			self.log.info("ByType: Searching for device with pin-id '%s'" % pin_id)

			for dev in devs:
				if dev.getDeviceAttributes('name')[0] == name and dev.getDeviceAttributes('pin_id')[0] == pin_id:
					matches.append(dev)

			for match in matches:
				devs.remove(match)
				current = current.getMergedDevice(match)

			if len(matches) == 0:
				self.log.info("ByType: no match for device: " + current.id.string)

			self.log.debug("ByType:\n\nResulting device: " + str(current))
			merged.append(current)

		return merged

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

			self.log.debug("ByType:\n\nResulting device: " + str(current))
			merged.append(current)

		return merged

	def _getCategoryTypeAVR(self, device):
		type = device.getDeviceAttributes('type')[0]
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

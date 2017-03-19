# -*- coding: utf-8 -*-
# Copyright (c) 2015, Roboterclub Aachen e.V.
# All rights reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

def getDefineForDevice(device_id, familyDefines):
	# get all defines for this device name
	devName = 'STM32{}{}'.format(device_id.family[0].upper(), device_id.name)

	# Map STM32F7x8 -> STM32F7x7
	if device_id.family == 'f7' and devName[8] == '8':
		devName = devName[:8] + '7'

	deviceDefines = sorted([define for define in familyDefines if define.startswith(devName)])
	# if there is only one define thats the one
	if len(deviceDefines) == 1:
		return deviceDefines[0]

	# now we match for the size-id.
	devNameMatch = devName + 'x{}'.format(device_id.size_id.upper())
	for define in deviceDefines:
		if devNameMatch <= define:
			return define

	# now we match for the pin-id.
	devNameMatch = devName + '{}x'.format(device_id.pin_id.upper())
	for define in deviceDefines:
		if devNameMatch <= define:
			return define

	return None

stm32f1_remaps = \
{
	# (position % 32) -> local bit position
	# MAPR register
	'spi1':			{'position':  0, 'mask': 1, 'mapping': [0, 1]},
	'i2c1':			{'position':  1, 'mask': 1, 'mapping': [0, 1]},
	'usart1':		{'position':  2, 'mask': 1, 'mapping': [0, 1]},
	'usart2':		{'position':  3, 'mask': 1, 'mapping': [0, 1]},
	'usart3':		{'position':  4, 'mask': 3, 'mapping': [0, 1,	3]},
	'tim1':			{'position':  6, 'mask': 3, 'mapping': [0, 1,	3]},
	'tim2':			{'position':  8, 'mask': 3, 'mapping': [0, 1, 2, 3]},
	'tim3':			{'position': 10, 'mask': 3, 'mapping': [0,	2, 3]},
	'tim4':			{'position': 12, 'mask': 1, 'mapping': [0, 1]},
	'can':			{'position': 13, 'mask': 3, 'mapping': [0,	2, 3]},
	'can1':			{'position': 13, 'mask': 3, 'mapping': [0,	2, 3]},
	'pd01':			{'position': 15, 'mask': 1, 'mapping': [0, 1]},
	'tim5ch4':		{'position': 16, 'mask': 1, 'mapping': [0, 1]},
	'adc1etrginj':	{'position': 17, 'mask': 1, 'mapping': [0, 1]},
	'adc1etrgreg':	{'position': 18, 'mask': 1, 'mapping': [0, 1]},
	'adc2etrginj':	{'position': 19, 'mask': 1, 'mapping': [0, 1]},
	'adc2etrgreg':	{'position': 20, 'mask': 1, 'mapping': [0, 1]},
	'eth':			{'position': 21, 'mask': 1, 'mapping': [0, 1]},
	'can2':			{'position': 22, 'mask': 1, 'mapping': [0, 1]},
	'mii_rmii_sel':	{'position': 23, 'mask': 1, 'mapping': [0, 1]},
	'swj_cfg':		{'position': 24, 'mask': 7, 'mapping': [0, 1, 2,	4]},
	# position 27 is empty
	'spi3':			{'position': 28, 'mask': 1, 'mapping': [0, 1]},
	'i2s3':			{'position': 28, 'mask': 1, 'mapping': [0, 1]},
	'tim2itr1':		{'position': 29, 'mask': 1, 'mapping': [0, 1]},
	'ptp_pps':		{'position': 30, 'mask': 1, 'mapping': [0, 1]},
	# position 31 is empty
	# MAPR2 register
	'tim15':		{'position': 32, 'mask': 1, 'mapping': [0, 1]},
	'tim16':		{'position': 33, 'mask': 1, 'mapping': [0, 1]},
	'tim17':		{'position': 34, 'mask': 1, 'mapping': [0, 1]},
	'cec':			{'position': 35, 'mask': 1, 'mapping': [0, 1]},
	'tim1_dma':		{'position': 36, 'mask': 1, 'mapping': [0, 1]},
	'tim9':			{'position': 37, 'mask': 1, 'mapping': [0, 1]},
	'tim10':		{'position': 38, 'mask': 1, 'mapping': [0, 1]},
	'tim11':		{'position': 39, 'mask': 1, 'mapping': [0, 1]},
	'tim13':		{'position': 40, 'mask': 1, 'mapping': [0, 1]},
	'tim14':		{'position': 41, 'mask': 1, 'mapping': [0, 1]},
	'fsmc_nadv':	{'position': 42, 'mask': 1, 'mapping': [0, 1]},
	'tim67_dac_dma':{'position': 43, 'mask': 1, 'mapping': [0, 1]},
	'tim12':		{'position': 44, 'mask': 1, 'mapping': [0, 1]},
	'misc':			{'position': 45, 'mask': 1, 'mapping': [0, 1]},
}

stm32_memory = \
{
	'f0': {
		'start': {
			'flash': 0x08000000,
			'sram': 0x20000000
		},
		'model': [
			{
				'name': ['030', '031', '038', '042', '048', '051', '058', '070', '071', '072', '078', '091', '098'],
				'memories': {'flash': 0, 'sram1': 0}
			}
		]
	},
	'f1': {
		'start': {
			'flash': 0x08000000,
			'sram': 0x20000000
		},
		'model': [
			{
				'name': ['100', '101', '102', '103', '105', '107'],
				'memories': {'flash': 0, 'sram1': 0}
			}
		]
	},
	'f2': {
		'start': {
			'flash': 0x08000000,
			'sram': 0x20000000
		},
		'model': [
			{
				'name': ['205', '207', '215', '217'],
				'memories': {'flash': 0, 'sram1': 0, 'sram2': 16}
			}
		]
	},
	'f3': {
		'start': {
			'flash': 0x08000000,
			'ccm': 0x10000000,
			'sram': 0x20000000
		},
		'model': [
			{
				'name' : ['301', '302', '318', '378', '373'],
				'memories' : {'flash': 0, 'sram1' : 0}
			},
			{
				'name': ['303x6', '303x8', '328', '334'],
				'memories': {'flash': 0, 'ccm': 4, 'sram1': 0}
			},
			{
				'name': ['303xb', '303xc', '358'],
				'memories': {'flash': 0, 'ccm': 8, 'sram1': 0}
			},
			{
				'name': ['303xd', '303xe', '398'],
				'memories': {'flash': 0, 'ccm': 16, 'sram1': 0}
			}
		]
	},
	'f4': {
		'start': {
			'flash': 0x08000000,
			'ccm': 0x10000000,
			'sram': 0x20000000,
			'backup': 0x40024000
		},
		'model': [
			{
				'name' : ['401', '410', '411', '412', '446'],
				'memories' : {'flash': 0, 'sram1' : 0}
			},
			{
				'name': ['405', '407', '415', '417'],
				'memories': {'flash': 0, 'ccm': 64, 'sram1': 0, 'sram2': 16, 'backup': 4}
			},
			{
				'name': ['413', '423'],
				'memories': {'flash': 0, 'ccm': 64, 'sram1': 0, 'backup': 4}
			},
			{
				'name': ['427', '429', '437', '439'],
				'memories': {'flash': 0, 'ccm': 64, 'sram1': 0, 'sram2': 16, 'sram3': 64, 'backup': 4}
			},
			{
				'name': ['469', '479'],
				'memories': {'flash': 0, 'ccm': 64, 'sram1': 0, 'sram2': 32, 'sram3': 128, 'backup': 4}
			}
		]
	},
	'f7': {
		'start': {
			'flash': 0x08000000,
			'dtcm': 0x20000000,
			'itcm': 0x00000000,
			'sram': 0x20010000,
			'backup': 0x40024000
		},
		'model': [
			{
				'name': ['722', '732', '723', '733', '745', '746', '756', '765', '767', '768', '769', '777', '778', '779'],
				'memories': {'flash': 0, 'itcm': 16, 'dtcm': 64, 'sram1': 0, 'sram2': 16, 'backup': 4}
			}
		]
	},
	'l0': {
		'start': {
			'flash': 0x08000000,
			'eeprom': 0x08080000,
			'sram': 0x20000000
		},
		'model': [
			{
				# CAT1
				'name': ['011', '021'],
				'memories': {'flash': 0, 'sram1': 0, 'eeprom': 1}
			},{
				# CAT2
				'name': ['031', '041'],
				'memories': {'flash': 0, 'sram1': 0, 'eeprom': 1}
			},{
				# CAT3
				'name': ['051', '052', '053', '062', '063'],
				'memories': {'flash': 0, 'sram1': 0, 'eeprom': 2}
			},{
				# CAT5
				'name': ['071', '072', '073', '081', '082', '083'],
				'memories': {'flash': 0, 'sram1': 0, 'eeprom': 6}
			},
		]
	},
	'l1': {
		'start': {
			'flash': 0x08000000,
			'eeprom': 0x08080000,
			'sram': 0x20000000
		},
		'model': [
			{
				# CAT1 & 2
				'name': ['100x6', '100x8', '100xb', '151x6', '151x8', '151xb', '152x6', '152x8', '152xb'],
				'size': ['6', '8', 'b'],
				'memories': {'flash': 0, 'sram1': 0, 'eeprom': 4}
			},{
				# CAT3
				'name': ['100xc', '151xc', '152xc', '162xc'],
				'size': ['c'],
				'memories': {'flash': 0, 'sram1': 0, 'eeprom': 8}
			},{
				# CAT4
				'name': ['151xd', '152xd', '162xd'],
				'size': ['d'],
				'memories': {'flash': 0, 'sram1': 0, 'eeprom': 12}
			},{
				# CAT5 & 6
				'name': ['151xe', '152xe', '162xe'],
				'size': ['e'],
				'memories': {'flash': 0, 'sram1': 0, 'eeprom': 16}
			},
		]
	},
	'l4': {
		'start': {
			'flash': 0x08000000,
			'eeprom': 0x08080000,
			'sram2': 0x10000000,
			'sram1': 0x20000000
		},
		'model': [
			{
				'name': ['431', '433', '443', '451', '471', '475', '476', '485', '486'],
				'memories': {'flash': 0, 'sram1': 0, 'sram2': 32}
			},{
				'name': ['432', '442', '452', '462'],
				'memories': {'flash': 0, 'sram1': 0, 'sram2': 16}
			},
		]
	},
}

def getMemoryForDevice(device_id):
	mem_fam = stm32_memory[device_id.family]
	mem_model = None
	for model in mem_fam['model']:
		if any(name.startswith(device_id.name) for name in model['name']):
			if device_id.name in model['name']:
				mem_model = model
				break
			elif "{}x{}".format(device_id.name, device_id.size_id) in model['name']:
				mem_model = model
				break
	if mem_model == None:
		print("Memory model not found for device '{}'".format(device_id.string))
		exit(1)
	return (mem_fam['start'], mem_model['memories'])

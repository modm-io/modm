# -*- coding: utf-8 -*-
# Copyright (c) 2015, Roboterclub Aachen e.V.
# All rights reserved.
#
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

stm32_defines = \
{
	'f0': [
		'STM32F030x6',	# STM32F030x4, STM32F030x6 Devices (STM32F030xx microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)
		'STM32F030x8',	# STM32F030x8 Devices (STM32F030xx microcontrollers where the Flash memory is 64 Kbytes)
		'STM32F031x6',	# STM32F031x4, STM32F031x6 Devices (STM32F031xx microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)
		'STM32F038xx',	# STM32F038xx Devices (STM32F038xx microcontrollers where the Flash memory is 32 Kbytes)
		'STM32F042x6',	# STM32F042x4, STM32F042x6 Devices (STM32F042xx microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)
		'STM32F048xx',	# STM32F048xx Devices (STM32F042xx microcontrollers where the Flash memory is 32 Kbytes)
		'STM32F051x8',	# STM32F051x4, STM32F051x6, STM32F051x8 Devices (STM32F051xx microcontrollers where the Flash memory ranges between 16 and 64 Kbytes)
		'STM32F058xx',	# STM32F058xx Devices (STM32F058xx microcontrollers where the Flash memory is 64 Kbytes)
		'STM32F070x6',	# STM32F070x6 Devices (STM32F070x6 microcontrollers where the Flash memory ranges between 16 and 32 Kbytes)
		'STM32F070xB',	# STM32F070xB Devices (STM32F070xB microcontrollers where the Flash memory ranges between 64 and 128 Kbytes)
		'STM32F071xB',	# STM32F071x8, STM32F071xB Devices (STM32F071xx microcontrollers where the Flash memory ranges between 64 and 128 Kbytes)
		'STM32F072xB',	# STM32F072x8, STM32F072xB Devices (STM32F072xx microcontrollers where the Flash memory ranges between 64 and 128 Kbytes)
		'STM32F078xx',	# STM32F078xx Devices (STM32F078xx microcontrollers where the Flash memory is 128 Kbytes)
		'STM32F030xC',	# STM32F030xC Devices (STM32F030xC microcontrollers where the Flash memory is 256 Kbytes)
		'STM32F091xC',	# STM32F091xC Devices (STM32F091xx microcontrollers where the Flash memory is 256 Kbytes)
		'STM32F098xx',	# STM32F098xx Devices (STM32F098xx microcontrollers where the Flash memory is 256 Kbytes)
	],
	'f1': [
		 'STM32F100xB',	# STM32F100C4, STM32F100R4, STM32F100C6, STM32F100R6, STM32F100C8, STM32F100R8, STM32F100V8, STM32F100CB, STM32F100RB and STM32F100VB
		 'STM32F100xE',	# STM32F100RC, STM32F100VC, STM32F100ZC, STM32F100RD, STM32F100VD, STM32F100ZD, STM32F100RE, STM32F100VE and STM32F100ZE
		 'STM32F101x6',	# STM32F101C4, STM32F101R4, STM32F101T4, STM32F101C6, STM32F101R6 and STM32F101T6 Devices
		 'STM32F101xB',	# STM32F101C8, STM32F101R8, STM32F101T8, STM32F101V8, STM32F101CB, STM32F101RB, STM32F101TB and STM32F101VB
		 'STM32F101xE',	# STM32F101RC, STM32F101VC, STM32F101ZC, STM32F101RD, STM32F101VD, STM32F101ZD, STM32F101RE, STM32F101VE and STM32F101ZE
		 'STM32F101xG',	# STM32F101RF, STM32F101VF, STM32F101ZF, STM32F101RG, STM32F101VG and STM32F101ZG
		 'STM32F102x6',	# STM32F102C4, STM32F102R4, STM32F102C6 and STM32F102R6
		 'STM32F102xB',	# STM32F102C8, STM32F102R8, STM32F102CB and STM32F102RB
		 'STM32F103x6',	# STM32F103C4, STM32F103R4, STM32F103T4, STM32F103C6, STM32F103R6 and STM32F103T6
		 'STM32F103xB',	# STM32F103C8, STM32F103R8, STM32F103T8, STM32F103V8, STM32F103CB, STM32F103RB, STM32F103TB and STM32F103VB
		 'STM32F103xE',	# STM32F103RC, STM32F103VC, STM32F103ZC, STM32F103RD, STM32F103VD, STM32F103ZD, STM32F103RE, STM32F103VE and STM32F103ZE
		 'STM32F103xG',	# STM32F103RF, STM32F103VF, STM32F103ZF, STM32F103RG, STM32F103VG and STM32F103ZG
		 'STM32F105xC',	# STM32F105R8, STM32F105V8, STM32F105RB, STM32F105VB, STM32F105RC and STM32F105VC
		 'STM32F107xC',	# STM32F107RB, STM32F107VB, STM32F107RC and STM32F107VC
	],
	'f2': [
		'STM32F205xx',
		'STM32F215xx',
		'STM32F207xx',
		'STM32F217xx',
	],
	'f3': [
		'STM32F301x8',	# STM32F301K6, STM32F301K8, STM32F301C6, STM32F301C8, STM32F301R6 and STM32F301R8 Devices
		'STM32F302x8',	# STM32F302K6, STM32F302K8, STM32F302C6, STM32F302C8, STM32F302R6 and STM32F302R8 Devices
		'STM32F302xC',	# STM32F302CB, STM32F302CC, STM32F302RB, STM32F302RC, STM32F302VB and STM32F302VC Devices
		'STM32F302xE',	# STM32F302CE, STM32F302RE, and STM32F302VE Devices
		'STM32F303x8',	# STM32F303K6, STM32F303K8, STM32F303C6, STM32F303C8, STM32F303R6 and STM32F303R8 Devices
		'STM32F303xC',	# STM32F303CB, STM32F303CC, STM32F303RB, STM32F303RC, STM32F303VB and STM32F303VC Devices
		'STM32F303xE',	# STM32F303RE, STM32F303VE and STM32F303ZE Devices
		'STM32F373xC',	# STM32F373C8, STM32F373CB, STM32F373CC, STM32F373R8, STM32F373RB, STM32F373RC, STM32F373V8, STM32F373VB and STM32F373VC Devices
		'STM32F334x8',	# STM32F334C4, STM32F334C6, STM32F334C8, STM32F334R4, STM32F334R6 and STM32F334R8 Devices
		'STM32F318xx',	# STM32F318K8, STM32F318C8: STM32F301x8 with regulator off: STM32F318xx Devices
		'STM32F328xx',	# STM32F328C8, STM32F328R8: STM32F334x8 with regulator off: STM32F328xx Devices
		'STM32F358xx',	# STM32F358CC, STM32F358RC, STM32F358VC: STM32F303xC with regulator off: STM32F358xx Devices
		'STM32F378xx',	# STM32F378CC, STM32F378RC, STM32F378VC: STM32F373xC with regulator off: STM32F378xx Devices
		'STM32F398xx',	# STM32F398CE, STM32F398RE, STM32F398VE: STM32F303xE with regulator off: STM32F398xx Devices
	],
	'f4': [
		'STM32F405xx',	# STM32F405RG, STM32F405VG and STM32F405ZG Devices
		'STM32F415xx',	# STM32F415RG, STM32F415VG and STM32F415ZG Devices
		'STM32F407xx',	# STM32F407VG, STM32F407VE, STM32F407ZG, STM32F407ZE, STM32F407IG and STM32F407IE Devices
		'STM32F417xx',	# STM32F417VG, STM32F417VE, STM32F417ZG, STM32F417ZE, STM32F417IG and STM32F417IE Devices
		'STM32F427xx',	# STM32F427VG, STM32F427VI, STM32F427ZG, STM32F427ZI, STM32F427IG and STM32F427II Devices
		'STM32F437xx',	# STM32F437VG, STM32F437VI, STM32F437ZG, STM32F437ZI, STM32F437IG and STM32F437II Devices
		'STM32F429xx',	# STM32F429VG, STM32F429VI, STM32F429ZG, STM32F429ZI, STM32F429BG, STM32F429BI, STM32F429NG, STM32F439NI, STM32F429IG and STM32F429II Devices
		'STM32F439xx',	# STM32F439VG, STM32F439VI, STM32F439ZG, STM32F439ZI, STM32F439BG, STM32F439BI, STM32F439NG, STM32F439NI, STM32F439IG and STM32F439II Devices
		'STM32F401xC',	# STM32F401CB, STM32F401CC, STM32F401RB, STM32F401RC, STM32F401VB and STM32F401VC Devices
		'STM32F401xE',	# STM32F401CD, STM32F401RD, STM32F401VD, STM32F401CE, STM32F401RE and STM32F401VE Devices
		'STM32F411xE',	# STM32F411CD, STM32F411RD, STM32F411VD, STM32F411CE, STM32F411RE and STM32F411VE Devices
		'STM32F446xx',	# STM32F446MC, STM32F446ME, STM32F446RC, STM32F446RE, STM32F446VC, STM32F446VE, STM32F446ZC and STM32F446ZE Devices
	],
	'f7': [
		'STM32F756xx',	# STM32F756VG, STM32F756ZG, STM32F756ZG, STM32F756IG, STM32F756BG and STM32F756NG Devices
		'STM32F746xx',	# STM32F746VE, STM32F746VG, STM32F746ZE, STM32F746ZG, STM32F746IE, STM32F746IG, STM32F746BE, STM32F746BG, STM32F746NE and STM32F746NG Devices
		'STM32F745xx',	# STM32F745VE, STM32F745VG, STM32F745ZG, STM32F745ZE, STM32F745IE and STM32F745IG Devices
	]
}

stm32f1_remaps = \
{
	'spi1':			{'mask': 1, 'position':  0, 'mapping': [0, 1]},
	'i2c1':			{'mask': 1, 'position':  1, 'mapping': [0, 1]},
	'usart1':		{'mask': 1, 'position':  2, 'mapping': [0, 1]},
	'usart2':		{'mask': 1, 'position':  3, 'mapping': [0, 1]},
	'usart3':		{'mask': 3, 'position':  4, 'mapping': [0, 1,    3]},
	'tim1':			{'mask': 3, 'position':  6, 'mapping': [0, 1,    3]},
	'tim2':			{'mask': 3, 'position':  8, 'mapping': [0, 1, 2, 3]},
	'tim3':			{'mask': 3, 'position': 10, 'mapping': [0,    2, 3]},
	'tim4':			{'mask': 1, 'position': 12, 'mapping': [0, 1]},
	'can':			{'mask': 3, 'position': 13, 'mapping': [0,    2, 3]},
	'can1': 		{'mask': 3, 'position': 13, 'mapping': [0, 2, 3]},
	'clock':		{'mask': 1, 'position': 15, 'mapping': [0, 1]},
	'tim5ch4':		{'mask': 1, 'position': 16, 'mapping': [0, 1]},
	'adc1etrginj':	{'mask': 1, 'position': 17, 'mapping': [0, 1]},
	'adc1etrgreg':	{'mask': 1, 'position': 18, 'mapping': [0, 1]},
	'adc2etrginj':	{'mask': 1, 'position': 19, 'mapping': [0, 1]},
	'adc2etrgreg':	{'mask': 1, 'position': 20, 'mapping': [0, 1]},
	'eth':			{'mask': 1, 'position': 21, 'mapping': [0, 1]},
	'can2':			{'mask': 1, 'position': 22, 'mapping': [0, 1]},
	'mii':			{'mask': 1, 'position': 23, 'mapping': [0, 1]},
	'swj':			{'mask': 7, 'position': 24, 'mapping': [0, 1, 2,    4]},
	'spi3':			{'mask': 1, 'position': 28, 'mapping': [0, 1]},
	'tim2itr1':		{'mask': 1, 'position': 29, 'mapping': [0, 1]},
	'ptp':			{'mask': 1, 'position': 30, 'mapping': [0, 1]}
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
				'names': ['030', '031', '038', '042', '048', '051', '058', '070', '071', '072', '078', '091', '098'],
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
				'names': ['100', '101', '102', '103', '105', '107'],
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
				'names': ['205', '207', '215', '217'],
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
				'names' : ['301', '302', '318', '378', '373'],
				'memories' : {'flash': 0, 'sram1' : 0}
			},
			{
				'names': ['303x6', '303x8', '328', '334', '373'],
				'memories': {'flash': 0, 'ccm': 4, 'sram1': 0}
			},
			{
				'names': ['303xb', '303xc', '358'],
				'memories': {'flash': 0, 'ccm': 8, 'sram1': 0}
			},
			{
				'names': ['303xd', '303xe', '398'],
				'memories': {'flash': 0, 'ccm': 16, 'sram1': 0}
			}
		]
	},
	'f4': {
		'start': {
			'flash': 0x08000000,
			'ccm': 0x10000000,
			'sram': 0x20000000
		},
		'model': [
			{
				'names' : ['401', '411', '446'],
				'memories' : {'flash': 0, 'sram1' : 0}
			},
			{
				'names': ['405', '407', '415', '417'],
				'memories': {'flash': 0, 'ccm': 64, 'sram1': 0, 'sram2': 16}
			},
			{
				'names': ['427', '429', '437', '439'],
				'memories': {'flash': 0, 'ccm': 64, 'sram1': 0, 'sram2': 16, 'sram3': 64}
			}
		]
	},
	'f7': {
		'start': {
			'flash': 0x08000000,
			'dtcm': 0x20000000,
			'itcm': 0x00200000,
			'sram': 0x20010000
		},
		'model': [
			{
				'names': ['745', '746', '756'],
				'memories': {'flash': 0, 'itcm': 16, 'dtcm': 64, 'sram1': 0, 'sram2': 16}
			}
		]
	}
}

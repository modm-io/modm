/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ST7565_DEFINES_HPP
#define XPCC__ST7565_DEFINES_HPP

// ----------------------------------------------------------------------------
// Defines for the ST7565 display controller.
// ----------------------------------------------------------------------------

//51mmx31mm
// Command table EA DOGM128-6 Page 5

namespace xpcc
{
	// (1) Display ON/OFF
	#define ST7565_ON       			0xAF
	#define ST7565_OFF      			0xAE
	
	// (2) Display start line set
	
	
	// (3) Page address set
	#define ST7565_PAGE_ADDRESS			0xB0
	
	// (4) Column address set upper bit
	#define ST7565_COL_ADDRESS_MSB		0x10
	// (4) Column address set lower bit
	#define ST7565_COL_ADDRESS_LSB		0x00
	
	// (5) Status read (doesn't work in SPI mode)
	
	// (6) Display data write
	
	// (7) Display data read (doesn't work in SPI mode)
	
	// (8) ADC select
	#define ST7565_ADC_NORMAL			0xA0
	#define ST7565_ADC_REVERSE			0xA1
	
	// (9) Display normale/reverse
	#define ST7565_NORMAL   			0xA6
	#define ST7565_REVERSE				0xA7
	
	// (10) Display all points ON/OFF
	#define ST7565_SHOW_NORMAL			0xA4
	#define ST7565_SHOW_ALL_POINTS		0xA5
	
	// (11) LCD bias set
	#define ST7565_BIAS_1_9				0xA2
	#define ST7565_BIAS_1_7				0xA3
	
	// (12) Read-modify-write (doesn't work in SPI mode)
	
	// (13) End clear read/modify/write (doesn't work in SPI mode)
	
	// (14) RESET
	#define ST7565_RESET_CMD			0xE2
	
	// (15) Common output mode select
	#define ST7565_SCAN_DIR_NORMAL		0xC0
	#define ST7565_SCAN_DIR_REVERSE		0xC8
	
	// (16) Power control set
	#define ST7565_POWER_CONTROL		0x28
	#define ST7565_POWER_LOW_POWER		0x2F
	#define ST7565_POWER_WIDE_RANGE		0x2F
	#define ST7565_POWER_LOW_VOLTAGE	0x2B
	
	// (17) V0 voltage regulator internal resistor ratio set
	#define ST7565_VOLTAGE				0x20
	
	// (18) Electronic volume mode set
	#define ST7565_VOLUME_MODE_1		0x81
	// (18) Register
	#define ST7565_VOLUME_MODE_2		0x00
	
	// (19) Static indicator ON/OFF
	#define ST7565_INDICATOR_ON			0xAD
	#define ST7565_INDICATOR_OFF		0xAC
	// (19) Static indicator register set
	#define ST7565_INDICATOR_MODE_OFF	0x00
	#define ST7565_INDICATOR_MODE_1HZ	0x01
	#define ST7565_INDICATOR_MODE_2HZ	0x10
	#define ST7565_INDICATOR_MODE_ON	0x11
	
	// (20) Booster ratio set
	#define ST7565_BOOSTER_SET			0xF8
	#define ST7565_BOOSTER_234			0x00
	#define ST7565_BOOSTER_5			0x01
	#define ST7565_BOOSTER_6			0x03
	
	// (21) Power save
	
	// (22) NOP
	#define ST7565_NOP					0xE3
}

#endif // XPCC__ST7565_DEFINES_HPP

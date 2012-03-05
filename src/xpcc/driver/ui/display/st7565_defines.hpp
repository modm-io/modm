// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: st7565_defines.hpp 607 2011-09-13 19:51:03Z dergraaf $
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

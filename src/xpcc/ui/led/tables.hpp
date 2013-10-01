// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_PWM_LED_TABLE_HPP
#define XPCC_PWM_LED_TABLE_HPP

#include <stdint.h>
#include <xpcc/architecture/driver/accessor/flash.hpp>

namespace xpcc
{

/**
 * These are lookup tables for gamma corrected values used in fading LEDs.
 * Most of the time, 256 (8bit) steps are enough for simple indication,
 * however, for color sensitive tasks (like RGB LEDs) consider using the
 * 1000 step table.
 * Only the tables you use will be copied into flash.
 *
 * If any special behavior is expected, you can copy and modify these
 * tables into your own project.
 * Most of these use a gamma of 2.0.
 */
namespace ui
{
	/// Provides 8bit resolution (0 to 255) in 256 steps, 256 bytes of flash.
	EXTERN_FLASH_STORAGE(uint8_t table8_256[256]);

	/// Provides 10bit resolution (0 to 1023) in 256 steps, 512 bytes of flash.
	EXTERN_FLASH_STORAGE(uint16_t table10_256[256]);

	/// Provides 12bit resolution (0 to 4095) in 256 steps, 512 bytes of flash.
	EXTERN_FLASH_STORAGE(uint16_t table12_256[256]);

	/// Provides 16bit resolution (0 to 65535) in 256 steps, 512 bytes of flash.
	EXTERN_FLASH_STORAGE(uint16_t table16_256[256]);
}

}

#endif	// XPCC_PWM_LED_TABLE_HPP

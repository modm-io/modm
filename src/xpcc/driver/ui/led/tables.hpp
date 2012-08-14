// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__PWM_LED_TABLE_HPP
#define XPCC__PWM_LED_TABLE_HPP

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
	namespace led
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

#endif	// XPCC__PWM_LED_TABLE_HPP

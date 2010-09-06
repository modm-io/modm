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
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/driver.hpp>

#include "xpcc_config.hpp"

namespace xpcc
{
	namespace st7565
	{
		FLASH(uint8_t configuration[14]) =
		{
			0x40, // Display start line 0
			
			// View direction
#if ST7565R_TOPVIEW == 1
			0xA0, // ADC normal
			0xC8, // reverse COM0~COM63
#else
			0xA1,
			0xC0,
#endif
			
			// Normal / Inverted
#if ST7565R_INVERTED == 1
			0xA7, // Inverted
#else
			0xA6, // Display normal
#endif
			
			// Hardware options
			0xA2, // Set bias 1/9 (Duty 1/65)
			0x2F, // Booster, Regulator and Follower on
			0xF8, // Set internal Booster to 4x
			0x00,
			
			// Contrast options
			0x27, // Contrast set
			0x81,
			0x16,
			
			// Indicator options
			0xAC, // No indicator
			0x00,
			
			// (Init done)
			0xAF, // Display on
		};
	}
}

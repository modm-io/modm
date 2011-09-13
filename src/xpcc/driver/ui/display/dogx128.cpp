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

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/driver.hpp>

#include "st7565_defines.hpp"

namespace xpcc
{
	namespace st7565
	{
		FLASH_STORAGE(uint8_t configuration_dogx128[12]) =
		{
			0x40, // Display start line 0
			
			// Normal / Inverted
			ST7565_NORMAL,				// Display normal
			
			// Hardware options
			ST7565_BIAS_1_9,			// Set bias 1/9 (Duty 1/65)
			ST7565_POWER_WIDE_RANGE,	// Booster, Regulator and Follower on
			ST7565_BOOSTER_SET,			// Set internal Booster to 4x
			ST7565_VOLUME_MODE_2,
			
			// Contrast options
			0x27,						// Contrast set
			ST7565_VOLUME_MODE_1,
			0x16,
			
			// Indicator options
			ST7565_INDICATOR_OFF, 		// No indicator
			ST7565_VOLUME_MODE_2,
			
			// (Init done)
			ST7565_ON, 					// Display on
		};
	}
}

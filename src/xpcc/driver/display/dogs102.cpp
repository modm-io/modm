/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/utils.hpp>
#include <modm/architecture/driver.hpp>

#include "st7565_defines.hpp"

namespace xpcc
{
	namespace st7565
	{
		FLASH_STORAGE(uint8_t configuration_dogs102[11]) =
		{
			0x40,	// Display start line 0
			0xa4,	// Display -> Set All Pixel to ON
			
			// Normal / Inverted
			ST7565_NORMAL,				// Display normal
			
			// Hardware options
			ST7565_BIAS_1_9,			// Set bias 1/9 (Duty 1/65)
			ST7565_POWER_WIDE_RANGE,	// Booster, Regulator and Follower on
			
			// Contrast options
			0x25,						// Contrast set
			ST7565_VOLUME_MODE_1,
			27,
			
			// Set Temperature compensation
			0xfa,
			0x90,
			
			// (Init done)
			ST7565_ON, 					// Display on
		};
	}
}

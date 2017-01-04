/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
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

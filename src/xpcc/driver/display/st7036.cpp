/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/driver.hpp>
#include "st7036.hpp"

#include "xpcc_config.hpp"

namespace xpcc
{
	namespace st7036
	{
		FLASH_STORAGE(uint8_t configuration[10]) =
		{
#if (ST7036_VOLTAGE == 3)

			0x39,
			0x14,
			0x55,
			0x6d,
			0x78,

#elif (ST7036_VOLTAGE == 5)

			0x39,
			0x1C,
			0x52,
			0x69,
			0x74,

#else
	#error "Unknown definition for 'ST7036_VOLTAGE', allowed values are '3' for 3,3V and '5' for 5V!"
#endif
			0x0f,
			0x01,		// clear display
			0x03,		// return to home position
			0x06,		// set cursor move direction
			0x0c,		// display on, disable cursor, no blink
		};
	}
}

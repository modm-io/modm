/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2015, Sascha Schade
 * Copyright (c) 2014, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// created with FontCreator 3.0

#include <modm/architecture/interface/accessor.hpp>

namespace modm
{
	namespace font
	{
		FLASH_STORAGE(uint8_t Matrix8x8[]) =
		{
			0x62, 0x00, // total size of this array
			8,	// width (may vary)
			8,	// height
			0,	// hspace
			0, 	// vspace
			48,	// first char
			10,	// char count

			// char widths
			// for each character the separate width in pixels
			 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,

			// font data
			// bit field of all characters
			0x3E, 0x7F, 0x71, 0x59, 0x4D, 0x7F, 0x3E, 0x00, // 48
			0x40, 0x42, 0x7F, 0x7F, 0x40, 0x40, 0x00, 0x00, // 49
			0x62, 0x73, 0x59, 0x49, 0x6F, 0x66, 0x00, 0x00, // 50
			0x22, 0x63, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00, // 51
			0x18, 0x1C, 0x16, 0x53, 0x7F, 0x7F, 0x50, 0x00, // 52
			0x27, 0x67, 0x45, 0x45, 0x7D, 0x39, 0x00, 0x00, // 53
			0x3C, 0x7E, 0x4B, 0x49, 0x79, 0x30, 0x00, 0x00, // 54
			0x03, 0x03, 0x71, 0x79, 0x0F, 0x07, 0x00, 0x00, // 55
			0x36, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00, // 56
			0x06, 0x4F, 0x49, 0x69, 0x3F, 0x1E, 0x00, 0x00, // 57
		};
	}
}


/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/interface/accessor.hpp>

namespace bitmap
{
	FLASH_STORAGE(uint8_t home_16x16[]) =
	{
		16, 16,
		0x00, 0xf8, 0x04, 0x82, 0xc2, 0x62, 0x72, 0x7a, 0x7a, 0x72, 0x62, 0xc2, 0x82, 0x04, 0xf8, 0x00,
		0x00, 0x1f, 0x20, 0x40, 0x5f, 0x50, 0x50, 0x5e, 0x5e, 0x5e, 0x50, 0x5f, 0x40, 0x20, 0x1f, 0x00,
	};
}


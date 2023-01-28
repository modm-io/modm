/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2014-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include "../device.hpp"

namespace modm::platform
{

/**
 * Get unique device id (96-bits)
 *
 * @param offset	0..2
 * @return	32-bit of the unique id
 * @ingroup modm_platform_id
 */
static inline uint32_t
getUniqueId(uint8_t offset)
{
	if (offset > 2) return 0;
	return *(((uint32_t *) UID_BASE) + offset);
}

}

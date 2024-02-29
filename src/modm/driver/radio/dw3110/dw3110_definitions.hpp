/*
 * Copyright (c) 2024, Elias H.
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DW3110_DEFINITIONS_HPP
#define MODM_DW3110_DEFINITIONS_HPP

#include <stdint.h>

#include <modm/architecture/interface/register.hpp>
#include <modm/math/utils/bit_constants.hpp>

namespace modm
{

/// @ingroup modm_driver_dw3110
struct Dw3110Register
{
	enum class SpiMode : uint8_t
	{
		FULL_READ = 0b01 << 6,
		FULL_WRITE = 0b11 << 6,
		SHORT_READ = 0b00 << 6,
		SHORT_WRITE = 0b10 << 6,
	};

	enum class DwRegister : uint16_t
	{
		DEV_ID = ((0x00 & 0x1F) << 9) | ((0x00 & 0x7F) << 2),  //< Device Identifier
	};
};

}  // namespace modm

#endif /* MODM_DW3110_DEFINITIONS_HPP */

/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
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
#include <cstdlib>

namespace modm
{

/// @ingroup modm_math_utils
/// @{

constexpr uint32_t
fromBcd(uint32_t bcd)
{
	uint32_t decimal = 0;
	for (uint16_t multiplier = 1; bcd; multiplier *= 10)
	{
		decimal += (bcd & 0b1111) * multiplier;
		bcd >>= 4;
	}
	return decimal;
}

constexpr uint32_t
toBcd(uint32_t decimal)
{
	uint32_t bcd = 0;
	for (uint16_t shift = 0; decimal; shift += 4)
	{
		const auto dv = std::div(decimal, 10l);
		bcd |= dv.rem << shift;
		decimal = dv.quot;
	}
	return bcd;
}

/// @}

} // namespace modm

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

#ifndef MODM_MATH_BCD_HPP
#define MODM_MATH_BCD_HPP

namespace modm
{

/// @ingroup modm_math_utils
/// @{

constexpr uint32_t
fromBcd(uint32_t bcd)
{
    uint32_t decimal = 0;
    for (uint16_t multiplier = 1; bcd != 0; multiplier *= 10)
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
    uint16_t remainder = decimal % 10;
    for (uint16_t shift = 0; decimal != 0; shift += 4)
    {
        bcd |= remainder << shift;
        decimal = (decimal - remainder) / 10;
        remainder = decimal % 10;
    }
    return bcd;
}

/// @}

} // namespace modm

#endif	// MODM_MATH_BCD_HPP

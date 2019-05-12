/*
 * Copyright (c) 2019, Niklas Hauser
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

namespace modm::math
{

/**
 * Slow, but table-less computation of CRC32.
 *
 * @see http://www.hackersdelight.org/hdcodetxt/crc.c.txt
 * @ingroup modm_math_utils
 */
inline uint32_t
crc32(const uint8_t *data, size_t length)
{
    uint32_t mask{0};
    uint32_t crc{0xFFFFFFFF};
    while (length--)
    {
        crc = crc ^ *data++;
        for (uint_fast8_t ii = 0; ii < 8; ii++)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
    }
    return ~crc;
}

} // namespace modm::math


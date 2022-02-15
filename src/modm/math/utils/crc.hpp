/*
 * Copyright (c) 2019-2020, Niklas Hauser
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
#include <stddef.h>
#ifdef __AVR__
#include <util/crc16.h>
#endif

namespace modm::math
{
/// @ingroup modm_math_utils
/// @{

inline uint8_t
crc8_ccitt_update(uint8_t crc, uint8_t data)
{
#ifdef __AVR__
    return _crc8_ccitt_update(crc, data);
#else
    data ^= crc;
    for (uint8_t ii = 0; ii < 8; ii++)
    {
        data <<= 1;
        if (data & 0x80) data ^= 0x07;
    }
    return data;
#endif
}

inline uint16_t
crc16_ccitt_update(uint16_t crc, uint8_t data)
{
#ifdef __AVR__
    return _crc_ccitt_update(crc, data);
#else
    data ^= uint8_t(crc); data ^= data << 4;
    return (((uint16_t(data) << 8) | uint8_t(crc >> 8)) ^
            uint8_t(data >> 4) ^ (uint16_t(data) << 3));
#endif
}

/// @see https://github.com/stbrumme/crc32
inline uint32_t
crc32_update(uint32_t crc, uint8_t data)
{
    static constexpr uint32_t polynomial{0xEDB88320};
    crc ^= data;
    for (uint_fast8_t ii = 0; ii < 8; ii++)
        crc = (crc >> 1) ^ (-int32_t(crc & 1) & polynomial);
    return crc;
}

static constexpr uint8_t crc8_ccitt_init{0xFFu};
static constexpr uint16_t crc16_ccitt_init{0xFFFFu};
static constexpr uint32_t crc32_init{0xFFFFFFFFul};

inline uint8_t
crc8_ccitt(const uint8_t *data, size_t length)
{
    uint8_t crc{crc8_ccitt_init};
    while (length--) crc = crc8_ccitt_update(crc, *data++);
    return crc;
}

inline uint16_t
crc16_ccitt(const uint8_t *data, size_t length)
{
    uint16_t crc{crc16_ccitt_init};
    while (length--) crc = crc16_ccitt_update(crc, *data++);
    return crc;
}

/// Slow, but table-less computation of CRC32.
inline uint32_t
crc32(const uint8_t *data, size_t length)
{
    uint32_t crc{crc32_init};
    while (length--) crc = crc32_update(crc, *data++);
    return ~crc;
}

/// @}
} // namespace modm::math


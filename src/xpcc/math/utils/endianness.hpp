/*
 * Copyright (c) 2013-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MATH_ENDIANNESS_HPP
#define XPCC_MATH_ENDIANNESS_HPP

#include <stdint.h>
#include "bit_operation.hpp"

#include <modm/architecture/detect.hpp>

namespace xpcc
{

/// checks if current architecture is big endian
/// @ingroup	math
constexpr bool
isBigEndian()
{
	return XPCC_IS_BIG_ENDIAN;
}

/// checks if current architecture is little endian
/// @ingroup	math
constexpr bool
isLittleEndian()
{
	return XPCC_IS_LITTLE_ENDIAN;
}

// MARK: - to host
/// @{
/// @ingroup	math
inline uint16_t
fromLittleEndian(uint16_t value)
{
#if XPCC_IS_LITTLE_ENDIAN
	return value;
#else
	return swap(value);
#endif
}

inline int16_t
fromLittleEndian(int16_t value)
{
	return int16_t(fromLittleEndian(uint16_t(value)));
}

inline uint32_t
fromLittleEndian(uint32_t value)
{
#if XPCC_IS_LITTLE_ENDIAN
	return value;
#else
	return swap(value);
#endif
}

inline int32_t
fromLittleEndian(int32_t value)
{
	return int32_t(fromLittleEndian(uint32_t(value)));
}

inline uint16_t
fromBigEndian(uint16_t value)
{
#if XPCC_IS_BIG_ENDIAN
	return value;
#else
	return swap(value);
#endif
}

inline int16_t
fromBigEndian(int16_t value)
{
	return int16_t(fromBigEndian(uint16_t(value)));
}

inline uint32_t
fromBigEndian(uint32_t value)
{
#if XPCC_IS_BIG_ENDIAN
	return value;
#else
	return swap(value);
#endif
}

inline int32_t
fromBigEndian(int32_t value)
{
	return int32_t(fromBigEndian(uint32_t(value)));
}

// MARK: - from host
inline uint16_t
toLittleEndian(uint16_t value)
{
#if XPCC_IS_LITTLE_ENDIAN
	return value;
#else
	return swap(value);
#endif
}

inline int16_t
toLittleEndian(int16_t value)
{
	return int16_t(toLittleEndian(uint16_t(value)));
}

inline uint32_t
toLittleEndian(uint32_t value)
{
#if XPCC_IS_LITTLE_ENDIAN
	return value;
#else
	return swap(value);
#endif
}

inline int32_t
toLittleEndian(int32_t value)
{
	return int32_t(toLittleEndian(uint32_t(value)));
}

inline uint16_t
toBigEndian(uint16_t value)
{
#if XPCC_IS_BIG_ENDIAN
	return value;
#else
	return swap(value);
#endif
}

inline int16_t
toBigEndian(int16_t value)
{
	return int16_t(toBigEndian(uint16_t(value)));
}

inline uint32_t
toBigEndian(uint32_t value)
{
#if XPCC_IS_BIG_ENDIAN
	return value;
#else
	return swap(value);
#endif
}

inline int32_t
toBigEndian(int32_t value)
{
	return int32_t(toBigEndian(uint32_t(value)));
}
/// @}

} // namespace xpcc

#endif // XPCC_MATH_ENDIANNESS_HPP

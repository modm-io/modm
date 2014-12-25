// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MATH_ENDIANNESS_HPP
#define XPCC_MATH_ENDIANNESS_HPP

#include <stdint.h>
#include "bit_operation.hpp"

#include <xpcc/architecture/detect.hpp>

namespace xpcc
{

constexpr bool
isBigEndian()
{
	return XPCC__IS_BIG_ENDIAN;
}

/// checks if current architecture is little endian
constexpr bool
isLittleEndian()
{
	return XPCC__IS_LITTLE_ENDIAN;
}

// MARK: - to host
inline uint16_t
fromLittleEndian(uint16_t value)
{
#if XPCC__IS_LITTLE_ENDIAN
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
#if XPCC__IS_LITTLE_ENDIAN
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
#if XPCC__IS_BIG_ENDIAN
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
#if XPCC__IS_BIG_ENDIAN
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
#if XPCC__IS_LITTLE_ENDIAN
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
#if XPCC__IS_LITTLE_ENDIAN
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
#if XPCC__IS_BIG_ENDIAN
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
#if XPCC__IS_BIG_ENDIAN
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

} // namespace xpcc

#endif // XPCC_MATH_ENDIANNESS_HPP

// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MATH_ENDIANESS_HPP
#define XPCC_MATH_ENDIANESS_HPP

#include <stdint.h>
#include "bit_operation.hpp"

namespace xpcc
{

namespace math
{

inline bool
isBigEndian()
{
	union
	{
		uint16_t i;
		char c[2];
	}
	bint = {0x0100};

	return (bint.c[0] == 0x01);
}

/// checks if current architecture is little endian
inline bool
isLittleEndian()
{
	return !isBigEndian();
}

// MARK: - to host
inline uint16_t
fromLittleEndian(uint16_t value)
{
	return isLittleEndian() ? value : swap(value);
}

inline int16_t
fromLittleEndian(int16_t value)
{
	return int16_t(fromLittleEndian(uint16_t(value)));
}

inline uint32_t
fromLittleEndian(uint32_t value)
{
	return isLittleEndian() ? value : swap(value);
}

inline int32_t
fromLittleEndian(int32_t value)
{
	return int32_t(fromLittleEndian(uint32_t(value)));
}

inline uint16_t
fromBigEndian(uint16_t value)
{
	return isBigEndian() ? value : swap(value);
}

inline int16_t
fromBigEndian(int16_t value)
{
	return int16_t(fromBigEndian(uint16_t(value)));
}

inline uint32_t
fromBigEndian(uint32_t value)
{
	return isBigEndian() ? value : swap(value);
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
	return isLittleEndian() ? value : swap(value);
}

inline int16_t
toLittleEndian(int16_t value)
{
	return int16_t(toLittleEndian(uint16_t(value)));
}

inline uint32_t
toLittleEndian(uint32_t value)
{
	return isLittleEndian() ? value : swap(value);
}

inline int32_t
toLittleEndian(int32_t value)
{
	return int32_t(toLittleEndian(uint32_t(value)));
}

inline uint16_t
toBigEndian(uint16_t value)
{
	return isBigEndian() ? value : swap(value);
}

inline int16_t
toBigEndian(int16_t value)
{
	return int16_t(toBigEndian(uint16_t(value)));
}

inline uint32_t
toBigEndian(uint32_t value)
{
	return isBigEndian() ? value : swap(value);
}

inline int32_t
toBigEndian(int32_t value)
{
	return int32_t(toBigEndian(uint32_t(value)));
}


} // namespace math

} // namespace xpcc

#endif // XPCC_MATH_ENDIANESS_HPP

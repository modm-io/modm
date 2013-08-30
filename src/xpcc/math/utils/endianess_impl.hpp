// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_MATH_ENDIANESS_HPP
#	error	"Don't include this file directly, use 'endianess.hpp' instead!"
#endif

#include <stdint.h>
#include "bit_operation.hpp"

inline uint16_t
xpcc::math::littleEndianToHost(uint16_t value)
{
	return isLittleEndian() ? value : swap(value);
}

inline uint32_t
xpcc::math::littleEndianToHost(uint32_t value)
{
	return isLittleEndian() ? value : swap(value);
}

inline uint16_t
xpcc::math::bigEndianToHost(uint16_t value)
{
	return isBigEndian() ? value : swap(value);
}

inline uint32_t
xpcc::math::bigEndianToHost(uint32_t value)
{
	return isBigEndian() ? value : swap(value);
}

inline uint16_t
xpcc::math::hostToLittleEndian(uint16_t value)
{
	return isLittleEndian() ? value : swap(value);
}

inline uint32_t
xpcc::math::hostToLittleEndian(uint32_t value)
{
	return isLittleEndian() ? value : swap(value);
}

inline uint16_t
xpcc::math::hostToBigEndian(uint16_t value)
{
	return isBigEndian() ? value : swap(value);
}

inline uint32_t
xpcc::math::hostToBigEndian(uint32_t value)
{
	return isBigEndian() ? value : swap(value);
}


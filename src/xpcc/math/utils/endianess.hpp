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
littleEndianToHost(uint16_t value);

inline uint32_t
littleEndianToHost(uint32_t value);

inline uint16_t
bigEndianToHost(uint16_t value);

inline uint32_t
bigEndianToHost(uint32_t value);

// MARK: - from host
inline uint16_t
hostToLittleEndian(uint16_t value);

inline uint32_t
hostToLittleEndian(uint32_t value);

inline uint16_t
hostToBigEndian(uint16_t value);

inline uint32_t
hostToBigEndian(uint32_t value);


} // namespace math

} // namespace xpcc

#include "endianess_impl.hpp"

#endif // XPCC_MATH_ENDIANESS_HPP

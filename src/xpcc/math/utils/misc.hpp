// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_MATH_UTILS_HPP
#define	XPCC_MATH_UTILS_HPP

#include <cstddef>
#include <cmath>
#include <stdint.h>

#include <xpcc/architecture/utils.hpp>

namespace xpcc
{

/**
 * Fast check if a float variable is positive
 *
 * Checks only the sign bit for the AVR.
 *
 * @ingroup	math
 */
inline bool
isPositive(const float& a)
{
	return !std::signbit(a);
}

// --------------------------------------------------------------------
/**
 * Compile time and runtime exponentiation
 *
 * Calculates base raised to the power of exponent.
 * If base and exponent are compile-time constants, the results is calculated
 * at compile time.
 * Otherwise the result is computed at runtime, which might be very expensive.
 *
 * @code
 * constexpr int value = xpcc::pow(10, 2);
 * @endcode
 *
 * @ingroup	math
 */
constexpr uint32_t
pow(uint32_t base, uint8_t exponent)
{
	return (exponent > 0) ? base * pow(base, exponent - 1) : 1;
}

}	// namespace xpcc

#endif	// XPCC_MATH_UTILS_HPP

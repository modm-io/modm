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

/**
 * @brief This does what you think it does.
 * @ingroup sorting_algorithms
 * @param  a  A thing of arbitrary type.
 * @param  b  Another thing of arbitrary type.
 * @return   The lesser of the parameters.
 *
 * This is the simple classic generic implementation.  It will work on
 * temporary expressions, since they are only evaluated once, unlike a
 * preprocessor macro.
 */
template<typename T>
inline const T&
min(const T& a, const T& b)
{
	if (b < a)
		return b;
	else
		return a;
}

/**
 * @brief This does what you think it does.
 * @ingroup sorting_algorithms
 * @param  a  A thing of arbitrary type.
 * @param  b  Another thing of arbitrary type.
 * @return   The greater of the parameters.
 *
 * This is the simple classic generic implementation.  It will work on
 * temporary expressions, since they are only evaluated once, unlike a
 * preprocessor macro.
 */
template<typename T>
inline const T&
max(const T& a, const T& b)
{
	if (a < b)
		return b;
	else
		return a;
}

/**
 * @brief This does what you think it does.
 * @ingroup sorting_algorithms
 * @param  a  A thing of arbitrary type.
 * @param  b  Another thing of arbitrary type.
 * @param  compare  A comparison functor.
 * @return   The lesser of the parameters.
 *
 * This will work on temporary expressions, since they are only evaluated
 * once, unlike a preprocessor macro.
 */
template<typename T, typename Compare>
inline const T&
min(const T& a, const T& b, Compare compare)
{
	if (compare(b, a))
		return b;
	else
		return a;
}

/**
 * @brief This does what you think it does.
 * @ingroup sorting_algorithms
 * @param  a  A thing of arbitrary type.
 * @param  b  Another thing of arbitrary type.
 * @param  compare  A comparison functor.
 * @return   The greater of the parameters.
 * 
 * This will work on temporary expressions, since they are only evaluated
 * once, unlike a preprocessor macro.
 */
template<typename T, typename Compare>
inline const T&
max(const T& a, const T& b, Compare compare)
{
	if (compare(a, b))
		return b;
	else
		return a;
}

}	// namespace xpcc

#endif	// XPCC_MATH_UTILS_HPP

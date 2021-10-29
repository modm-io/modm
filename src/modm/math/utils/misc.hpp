/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2012, 2014-2015, Niklas Hauser
 * Copyright (c) 2015, Sascha Schade
 * Copyright (c) 2020, Christopher Durand
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_MATH_UTILS_MISC_HPP
#define	MODM_MATH_UTILS_MISC_HPP

#include <stdint.h>

#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <modm/architecture/utils.hpp>

namespace modm
{

/// @addtogroup modm_math_utils
/// @{

/**
 * Fast check if a float variable is positive
 *
 * Checks only the sign bit for the AVR.
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
 * constexpr int value = modm::pow(10, 2);
 * @endcode
 */
constexpr uint32_t
pow(uint32_t base, uint8_t exponent)
{
	return (exponent > 0) ? base * pow(base, exponent - 1) : 1;
}

/**
 * @brief 		Variadic min for 2-∞ objects
 *
 * @param  a 	first object to compare
 * @param  b  	second object to compare
 * @param  cs  	More optional objects to compare
 * @return   	The smallest object
 *
 * @see			https://stackoverflow.com/questions/23815138/implementing-variadic-min-max-functions
 */
template<typename T>
constexpr T vmin(T a, T b)
{
    return a < b ? a : b;
}

template<typename T, typename... Ts>
constexpr T vmin(T a, T b, Ts&&... cs)
{
    return a < b ?
        vmin(a, std::forward<Ts>(cs)...) :
            vmin(b, std::forward<Ts>(cs)...);
}

/**
 * @brief 		Variadic max for 2-∞ objects
 *
 * @param  a 	first object to compare
 * @param  b  	second object to compare
 * @param  cs  	More optional objects to compare
 * @return   	The greatest object
 *
 * @see			https://stackoverflow.com/questions/23815138/implementing-variadic-min-max-functions
 */
template<typename T>
constexpr T vmax(T& a, T& b)
{
    return a > b ? a : b;
}

template<typename T, typename... Ts>
constexpr T vmax(T& a, T& b, Ts&... cs)
{
    return a > b ?
        vmax(a, std::forward<Ts>(cs)...) :
            vmax(b, std::forward<Ts>(cs)...);
}

/**
 * @brief This does what you think it does.
 *
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
	return compare(b, a) ? b : a;
}

/**
 * @brief This does what you think it does.
 *
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
	return compare(a, b) ? b : a;
}

/**
 * @brief constexpr implementation of fabs
 */
template <std::floating_point Float>
constexpr Float constexpr_fabs(Float number)
{
	return number >= 0 ? number : -number;
}

/// @}

}	// namespace modm

#endif

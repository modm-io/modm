/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2015, Thorsten Lajewski
 * Copyright (c) 2017, Daniel Krebs
 * Copyright (c) 2018, Christopher Durand
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <cstddef>
#include <cstdint>

#include <concepts>
#include <span>
#include <algorithm>
#include <numeric>
#include <bit>

#include <modm/math/utils/integer_traits.hpp>

namespace modm::filter
{

/**
 * \brief	Moving average filter
 *
 * Calculates the average of N newest values, i.a. the sum of the last N
 * values have been passed to update(...), divided by N. If less than N
 * values have been passed to the filter, the division factor is still N,
 * so missing values are assumed to be zero.
 *
 * For integer types this implementation stores the current sum of all values in the buffer
 * and updates this value with every call of update() by subtracting
 * the overwritten buffer index and adding the new one.
 *
 * Due to numerical instabillity for floating value types, inside the update function
 * the sum has to be recalculated making it less efficient.
 *
 * The internal sum is always up to date and the getValue()
 * method consists of only one division.
 *
 * \warning	Input range is limited by the following equation
 * 			\code N * input::maxValue < T::maxValue \endcode
 * 			The sum off the last N input values must not be greater than
 * 			the maximum value of T, otherwise an overflow will occur.
 *
 * \tparam	T	Input type
 * \tparam	N	Number of samples
 *
 * \ingroup	modm_math_filter
 */
template<typename T, std::size_t N>
class MovingAverage
{
public:
	constexpr MovingAverage(T initialValue = 0)
	{
		reset(initialValue);
	}

	/// Reset whole buffer to 'input'
	/// Next call of getValue() returns 'input'
	constexpr void reset(T input)
	{
		std::fill(std::begin(buffer), std::end(buffer), input);
		sum = N * input;
	}

	/// Append new value
	constexpr void
	update(T input)
	{
		if constexpr(std::floating_point<T>) {
			buffer[index] = input;
			sum = std::accumulate(std::begin(buffer), std::end(buffer), T{0});
		} else {
			sum -= buffer[index];
			sum += input;
			buffer[index] = input;
		}

		if (++index == N)
			index = 0;
	}

	/// Get filtered value
	T
	constexpr getValue() const
	{
		return (sum / static_cast<T>(N));
	}

private:
	least_uint<std::bit_width(N)> index{0};
	T buffer[N];
	T sum;
};

} // namespace modm::filter

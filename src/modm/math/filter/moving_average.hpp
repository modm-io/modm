/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2015, Thorsten Lajewski
 * Copyright (c) 2017, Daniel Krebs
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MOVING_AVERAGE_HPP
#define MODM_MOVING_AVERAGE_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace modm
{
	namespace filter{
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
		 * \tparam	N	Number of samples (maximum is 65356 or 2**16)
		 *
		 * \ingroup	modm_math_filter
		 */
		template<typename T, std::size_t N>
		class MovingAverage
		{
		private:
			using Index = std::conditional_t<
				(N >= 256),
				uint_fast16_t,
				uint_fast8_t
			>;

		public:
			MovingAverage(const T& initialValue = 0);

			/// Append new value
			void
			update(const T& input);

			/// Get filtered value
			const T
			getValue() const;

		private:
			Index index;
			T buffer[N];
			T sum;
		};
	}
}

// ----------------------------------------------------------------------------
template<typename T, std::size_t N>
modm::filter::MovingAverage<T, N>::MovingAverage(const T& initialValue) :
	index(0), sum(N * initialValue)
{
	for (Index i = 0; i < N; ++i) {
		buffer[i] = initialValue;
	}
}

// ----------------------------------------------------------------------------
// TODO implementierung für float anpassen
template<typename T, std::size_t N>
void
modm::filter::MovingAverage<T, N>::update(const T& input)
{
	sum -= buffer[index];
	sum += input;

	buffer[index] = input;

	index++;
	if (index >= N) {
		index = 0;
	}
}

// -----------------------------------------------------------------------------


template<typename T, std::size_t N>
const T
modm::filter::MovingAverage<T, N>::getValue() const
{
	return (sum / static_cast<T>(N));
}


#include "moving_average_float_impl.hpp"
#endif // MODM_MOVING_AVERAGE_HPP

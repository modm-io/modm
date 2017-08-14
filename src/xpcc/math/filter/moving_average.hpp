// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__MOVING_AVERAGE_HPP
#define XPCC__MOVING_AVERAGE_HPP

#include <cstddef>
#include <stdint.h>
#include <xpcc/utils/template_metaprogramming.hpp>

namespace xpcc
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
		 * \ingroup	filter
		 */
		template<typename T, std::size_t N>
		class MovingAverage
		{
		private:
			typedef typename ::xpcc::tmp::Select<
				(N >= 256),
				uint_fast16_t,
				uint_fast8_t >::Result Index;

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
xpcc::filter::MovingAverage<T, N>::MovingAverage(const T& initialValue) :
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
xpcc::filter::MovingAverage<T, N>::update(const T& input)
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
xpcc::filter::MovingAverage<T, N>::getValue() const
{
	return (sum / static_cast<T>(N));
}


#include "moving_average_float_impl.hpp"
#endif // XPCC__MOVING_AVERAGE_HPP

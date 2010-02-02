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
 *
 * $Id: moving_average.hpp 76 2009-10-14 23:29:28Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__MOVING_AVERAGE_HPP
#define XPCC__MOVING_AVERAGE_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \brief	Moving average filter
	 * 
	 * Input range: N * input::maxValue < T::maxValue
	 *
	 * \tparam	T	Input type
	 * \tparam	N	Number of samples (maximum 255)
	 * 
	 * \ingroup	filter
	 * \todo	documentation
	 */
	template<typename T, unsigned int N>
	class MovingAverage
	{
	public:
		MovingAverage(const T& initialValue = 0);
		
		void
		update(const T& input);
		
		const T
		getValue() const
		{
			return (sum / N);
		}
	
	private:
		uint8_t index;
		T buffer[N];
		T sum;
	};
}

// ----------------------------------------------------------------------------
template<typename T, unsigned int N>
xpcc::MovingAverage<T, N>::MovingAverage(const T& initialValue) :
	index(0), sum(N * initialValue)
{
	for (uint8_t i = 0; i < N; i++) {
		buffer[i] = initialValue;
	}
}

// ----------------------------------------------------------------------------
template<typename T, unsigned int N>
void
xpcc::MovingAverage<T, N>::update(const T& input)
{
	sum -= buffer[index];
	sum += input;
	buffer[index] = input;
	
	index++;
	if (index >= N) {
		index = 0;
	}
}

#endif // XPCC__MOVING_AVERAGE_HPP

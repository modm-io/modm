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
	#error	"Don't include this file directly, use 'moving_average.hpp' instead!"
#endif

namespace xpcc
{
	namespace filter{
		template<std::size_t N>
		class MovingAverage<double, N >
		{
		private:
			typedef typename ::xpcc::tmp::Select<
				(N >= 256),
				uint_fast16_t,
				uint_fast8_t >::Result Index;

		public:

			MovingAverage(const double& initialValue = 0);

			/// Append new value
			void
			update(const double& input);

			/// Get filtered value
			double
			getValue() const;

		private:
			Index index;
			double buffer[N];
			double sum;
		};
	}
}

//---------------------------------------------------------------------------------

template<std::size_t N>
xpcc::filter::MovingAverage<double, N>::MovingAverage(const double& initialValue) :
	index(0), sum(N * initialValue)
{
	for (Index i = 0; i < N; ++i) {
		buffer[i] = initialValue;
	}
}


//---------------------------------------------------------------------------------
template<std::size_t N>
void
xpcc::filter::MovingAverage<double, N>::update(const double& input){
	buffer[index] = input;

	index++;
	if (index >= N) {
		index = 0;
	}

	sum = 0;
	for (Index i = 0; i < N; ++i) {
		sum+= buffer[i];
	}

}

// ------------------------------------------------------------------------------
template<std::size_t N>
double
xpcc::filter::MovingAverage<double, N>::getValue() const
{
	return (sum / N);
}

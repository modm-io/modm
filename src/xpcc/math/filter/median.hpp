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
 * $Id: median.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FILTER__MEDIAN_HPP
#define XPCC_FILTER__MEDIAN_HPP

#include <stdint.h>

namespace xpcc
{
	namespace filter
	{
		/**
		 * \brief	Median filter
		 * 
		 * Calculates the median of a input set. Useful for eliminating spikes
		 * from the input. Adds a group delay of N/2 ticks for the signal.
		 * 
		 * Implementation are available for N = 3, 5, 7 and 9. To find
		 * the median the signal values will be partly sorted, but only as much
		 * as needed to find the median.
		 * 
		 * \code
		 * // create a new filter for five samples
		 * xpcc::filter::Median<uint8_t, 5> filter;
		 * 
		 * // append new signal values
		 * filter.append(10);
		 * filter.append(10);
		 * filter.append(20);
		 * 
		 * // calculate the median
		 * filter.update();
		 * 
		 * output = filter.getValue();
		 * \endcode
		 * 
		 * \tparam	T	Input type
		 * \tparam	N	Number of samples
		 * 
		 * \ingroup	filter
		 */
		template<typename T, int N>
		class Median
		{
		public:
			/**
			 * \brief	Constructor
			 * 
			 * \param	initialValue	Value will be set for the complete
			 * 							input buffer.
			 */
			Median(const T& initialValue = 0);
			
			/// Append new value
			void
			append(const T& input);
			
			/// calculate median
			void
			udpate();
			
			/// Get median value
			const T
			getValue() const;
		};
	}
}

#include "median_impl.hpp"

#endif // XPCC_FILTER__MEDIAN_HPP

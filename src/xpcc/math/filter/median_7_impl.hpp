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

namespace xpcc
{
	namespace filter
	{
		template <typename T>
		class Median<T, 7>
		{
		public:
			Median(const T& initialValue = 0);
			
			void
			append(const T& input);
			
			void
			update();
			
			const T
			getValue() const;
		
		private:
			uint_fast8_t index;
			T buffer[7];
			T sorted[7];
		};
	}
}

// ----------------------------------------------------------------------------
template <typename T>
xpcc::filter::Median<T, 7>::Median(const T& initialValue) :
	index(0)
{
	for (uint_fast8_t i = 0; i < 7; ++i) {
		buffer[i] = initialValue;
		sorted[i] = initialValue;
	}
}

template <typename T>
void
xpcc::filter::Median<T, 7>::append(const T& input)
{
	// Remove the old value from the sorted set and replace it with the
	// new input. This way the array remains mostly sorted.
	T valueToRemove = this->buffer[index];
	for (uint_fast8_t i = 0; i < 7; ++i)
	{
		if (this->sorted[i] == valueToRemove) {
			this->sorted[i] = input;
			break;
		}
	}
	
	this->buffer[index] = input;
	if (++index >= 7) {
		index = 0;
	}
}

template <typename T>
void
xpcc::filter::Median<T, 7>::update()
{
	XPCC_MEDIAN__SORT(sorted[0], sorted[5]);
	XPCC_MEDIAN__SORT(sorted[0], sorted[3]);
	XPCC_MEDIAN__SORT(sorted[1], sorted[6]);
	XPCC_MEDIAN__SORT(sorted[2], sorted[4]);
	XPCC_MEDIAN__SORT(sorted[0], sorted[1]);
	XPCC_MEDIAN__SORT(sorted[3], sorted[5]);
	XPCC_MEDIAN__SORT(sorted[2], sorted[6]);
	XPCC_MEDIAN__SORT(sorted[2], sorted[3]);
	XPCC_MEDIAN__SORT(sorted[3], sorted[6]);
	XPCC_MEDIAN__SORT(sorted[4], sorted[5]);
	XPCC_MEDIAN__SORT(sorted[1], sorted[4]);
	XPCC_MEDIAN__SORT(sorted[1], sorted[3]);
	XPCC_MEDIAN__SORT(sorted[3], sorted[4]);
}

template <typename T>
const T
xpcc::filter::Median<T, 7>::getValue() const
{
	return sorted[3];
}

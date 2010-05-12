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
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <string.h>

// ----------------------------------------------------------------------------
namespace xpcc
{
	namespace filter
	{
		template <typename T>
		class Median<T, 3>
		{
		public:
			Median(const T& initialValue = 0);
			
			void
			update(const T& input);
			
			const T
			getValue() const;
		
		private:
			uint8_t index;
			T buffer[3];
			T sorted[3];
		};
	}
}

// ----------------------------------------------------------------------------
template <typename T>
xpcc::filter::Median<T, 3>::Median(const T& initialValue) :
	index(0)
{
	for (uint_fast8_t i = 0; i < 3; ++i) {
		buffer[i] = initialValue;
		sorted[i] = initialValue;
	}
}

template <typename T>
void
xpcc::filter::Median<T, 3>::update(const T& input)
{
	this->buffer[index] = input;
	if (++index >= 3) {
		index = 0;
	}
	
	memcpy((void *) sorted, (const void *) this->buffer, sizeof(sorted));
	
	XPCC_MEDIAN__SORT(sorted[0],sorted[1]);
	XPCC_MEDIAN__SORT(sorted[1],sorted[2]);
	XPCC_MEDIAN__SORT(sorted[0],sorted[1]);
}

template <typename T>
const T
xpcc::filter::Median<T, 3>::getValue() const
{
	return sorted[1];
}

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

#ifndef XPCC_FILTER__DEBOUNCE_HPP
	#error	"Don't include this file directly, use 'debounce.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::filter::Debounce<T>::Debounce(const T& maxValue, const T& lowerBound, const T& upperBound) : 
	maxValue(maxValue),
	sum(maxValue / 2),
	lowerBound(lowerBound),
	upperBound((upperBound != 0) ? upperBound : maxValue),
	state(false)
{
}

template<typename T>
void
xpcc::filter::Debounce<T>::update(bool input)
{
	if (input && (sum < maxValue)) {
		sum++;
	}
	else if (!input && (sum > 0)) {
		sum--;
	}
	
	if (state && (sum <= lowerBound)) {
		state = false;
	}
	else if (!state && (sum >= upperBound)) {
		state = true;
	}
}

template<typename T>
bool
xpcc::filter::Debounce<T>::getValue() const
{
	return state;
}

template<typename T>
void
xpcc::filter::Debounce<T>::reset(const bool state)
{
	if (state) {
		sum = maxValue;
		this->state = true;
	}
	else {
		sum = 0;
		this->state = false;
	}
}

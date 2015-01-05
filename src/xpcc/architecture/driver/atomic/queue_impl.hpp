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

#ifndef	XPCC_ATOMIC__QUEUE_IMPL_HPP
#define	XPCC_ATOMIC__QUEUE_IMPL_HPP

#include <xpcc/architecture/detect.hpp>

template<typename T, std::size_t N>
xpcc::atomic::Queue<T, N>::Queue() :
	head(0), tail(0)
{
#if defined(XPCC__CPU_AVR)
	static_assert(N <= 254, "A maximum of 254 elements is allowed for AVRs!");
#endif
}

template<typename T, std::size_t N>
ALWAYS_INLINE bool
xpcc::atomic::Queue<T, N>::isFull() const
{
	Index tmphead = xpcc::accessor::asVolatile(this->head) + 1;
	if (tmphead >= (N+1)) {
		tmphead = 0;
	}
	
	return (tmphead == xpcc::accessor::asVolatile(this->tail));
}

template<typename T, std::size_t N>
bool
xpcc::atomic::Queue<T, N>::isNearlyFull() const
{
	static_assert(N > 3, "Not possible the check for 'nearly full' of such a small queue.");
	
	Index tmphead = xpcc::accessor::asVolatile(this->head);
	Index tmptail = xpcc::accessor::asVolatile(this->tail);

	Index free;
	if (tmphead >= tmptail) {
		free = (N + 1) - tmphead + tmptail;
	}
	else {
		free = tmptail - tmphead;
	}

	return (free < 3);
}

template<typename T, std::size_t N>
ALWAYS_INLINE bool
xpcc::atomic::Queue<T, N>::isEmpty() const
{
	return (xpcc::accessor::asVolatile(this->head) == xpcc::accessor::asVolatile(this->tail));
}

template<typename T, std::size_t N>
bool
xpcc::atomic::Queue<T, N>::isNearlyEmpty() const
{
	static_assert(N > 3, "Not possible the check for 'nearly empty' of such a small queue. ");

	uint8_t tmphead = xpcc::accessor::asVolatile(this->head);
	uint8_t tmptail = xpcc::accessor::asVolatile(this->tail);

	Index stored;
	if (tmphead >= tmptail) {
		stored = tmphead - tmptail;
	}
	else {
		stored = (N + 1) - tmptail + tmphead;
	}

	return (stored < 3);
}


template<typename T, std::size_t N>
ALWAYS_INLINE typename xpcc::atomic::Queue<T, N>::Size
xpcc::atomic::Queue<T, N>::getMaxSize() const
{
	return N;
}

template<typename T, std::size_t N>
ALWAYS_INLINE const T&
xpcc::atomic::Queue<T, N>::get() const
{
	return this->buffer[this->tail];
}

template<typename T, std::size_t N>
ALWAYS_INLINE bool
xpcc::atomic::Queue<T, N>::push(const T& value)
{
	Index tmphead = this->head + 1;
	if (tmphead >= (N+1)) {
		tmphead = 0;
	}
	if (tmphead == xpcc::accessor::asVolatile(this->tail)) {
		return false;
	}
	else {
		this->buffer[this->head] = value;
		this->head = tmphead;
		return true;
	}
}

template<typename T, std::size_t N>
ALWAYS_INLINE void
xpcc::atomic::Queue<T, N>::pop()
{
	Index tmptail = this->tail + 1;
	if (tmptail >= (N+1)) {
		tmptail = 0;
	}
	this->tail = tmptail;
}

#endif	// XPCC_ATOMIC__QUEUE_IMPL_HPP

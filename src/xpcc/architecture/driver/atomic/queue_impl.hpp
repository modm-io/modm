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

#ifndef	XPCC_ATOMIC__QUEUE_IMPL_HPP
#define	XPCC_ATOMIC__QUEUE_IMPL_HPP

template<typename T, std::size_t N>
xpcc::atomic::Queue<T, N>::Queue() :
	head(0), tail(0)
{
}

template<typename T, std::size_t N>
bool
xpcc::atomic::Queue<T, N>::isFull()
{
	uint8_t tmphead = xpcc::accessor::asVolatile(this->head) + 1;
	if (tmphead >= (N+1)) {
		tmphead = 0;
	}
	
	return (tmphead == xpcc::accessor::asVolatile(this->tail));
}

template<typename T, std::size_t N>
bool
xpcc::atomic::Queue<T, N>::isNearFull()
{
	XPCC__STATIC_ASSERT(N > 3, "Not possible the check for 'near full' of such a small queue. ");
	
	uint8_t tmphead = xpcc::accessor::asVolatile(this->head);
	uint8_t tmptail = xpcc::accessor::asVolatile(this->tail);

	uint8_t free;
	if (tmphead >= tmptail)
		free = N - tmphead + tmptail;
	else
		free = N - tmptail + tmphead;

	return (free < 3);
}

template<typename T, std::size_t N>
bool
xpcc::atomic::Queue<T, N>::isEmpty()
{
	return (xpcc::accessor::asVolatile(this->head) == xpcc::accessor::asVolatile(this->tail));
}

template<typename T, std::size_t N>
uint8_t
xpcc::atomic::Queue<T, N>::getMaxSize()
{
	return N;
}

template<typename T, std::size_t N>
const T&
xpcc::atomic::Queue<T, N>::get() const
{
	return this->buffer[this->tail];
}

template<typename T, std::size_t N>
bool
xpcc::atomic::Queue<T, N>::push(const T& value)
{
	uint8_t tmphead = this->head + 1;
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
void
xpcc::atomic::Queue<T, N>::pop()
{
	uint8_t tmptail = this->tail + 1;
	if (tmptail >= (N+1)) {
		tmptail = 0;
	}
	this->tail = tmptail;
}

#endif	// XPCC_ATOMIC__QUEUE_IMPL_HPP

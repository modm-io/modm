
// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#ifndef	XPCC_ISR_QUEUE_IMPL_HPP
#define	XPCC_ISR_QUEUE_IMPL_HPP

#define VOLATILE(x)  (*(volatile uint8_t *) &(x))

template<typename T, int N>
xpcc::IsrQueue<T, N>::IsrQueue() :
	head(0), tail(0) {
}

template<typename T, int N>
bool
xpcc::IsrQueue<T, N>::isFull() {
	uint8_t tmphead = VOLATILE(head) + 1;
	if (tmphead >= N) {
		tmphead = 0;
	}
	
	if (tmphead == VOLATILE(tail)) {
		return true;
	}
	return false;
}

template<typename T, int N>
bool
xpcc::IsrQueue<T, N>::isEmpty() {
	return (VOLATILE(head) == VOLATILE(tail));
}

template<typename T, int N>
uint8_t
xpcc::IsrQueue<T, N>::getMaxSize() {
	return N;
}

template<typename T, int N>
const T&
xpcc::IsrQueue<T, N>::get() const {
	return buffer[tail];
}

template<typename T, int N>
bool
xpcc::IsrQueue<T, N>::push(const T& value) {
	uint8_t tmphead = head + 1;
	if (tmphead >= N) {
		tmphead = 0;
	}
	if (tmphead == VOLATILE(tail)) {
		return false;
	}
	else {
		buffer[head] = value;
		head = tmphead;
		return true;
	}
}

template<typename T, int N>
void
xpcc::IsrQueue<T, N>::pop() {
	uint8_t tmptail = tail + 1;
	if (tmptail >= N) {
		tmptail = 0;
	}
	tail = tmptail;
}

#endif	// XPCC_ISR_QUEUE_IMPL_HPP

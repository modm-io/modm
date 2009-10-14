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

#ifndef	XPCC__DEQUE_HPP
	#error	"Don't include this file directly use 'data_structure/deque.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
bool
xpcc::BoundedDeque<T, N, S>::isEmpty() const {
	return (head == tail);
}

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
bool
xpcc::BoundedDeque<T, N, S>::isFull() const
{
	S tmphead = head + 1;
	if (tmphead >= N) {
		tmphead = 0;
	}
	return (tmphead == tail);
}

// ----------------------------------------------------------------------------
/*template<typename T, int N, typename S>
SizeType
xpcc::BoundedDeque<T, N, S>::getSize() const {
	return ;
}*/

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
S
xpcc::BoundedDeque<T, N, S>::getMaxSize() const {
	return N;
}

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
T&
xpcc::BoundedDeque<T, N, S>::front() {
	return buffer[tail];
}

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
const T&
xpcc::BoundedDeque<T, N, S>::front() const {
	return buffer[tail];
}

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
T&
xpcc::BoundedDeque<T, N, S>::back() {
	S tmphead = head - 1;
	if (tmphead >= N) {
		tmphead = N - 1;
	}
	return buffer[tmphead];
}

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
const T&
xpcc::BoundedDeque<T, N, S>::back() const {
	S tmphead = head - 1;
	if (tmphead >= N) {
		tmphead = N - 1;
	}
	return buffer[tmphead];
}


// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
bool
xpcc::BoundedDeque<T, N, S>::pushBack(const T& value) {
	// calculate the new buffer index
	S tmphead = head + 1;
	if (tmphead >= N) {
		tmphead = 0;
	}
	
	// check if there is enough space left
	if (tmphead == tail) {
		return false;
	}
	
	// save data and store new buffer index
	buffer[head] = value;
	head = tmphead;
	
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
bool
xpcc::BoundedDeque<T, N, S>::pushFront(const T& value) {
	// calculate the new buffer index
	S tmptail = tail - 1;
	if (tmptail >= N) {
		tmptail = N - 1;
	}
	
	// check if there is enough space left
	if (tmptail == head) {
		return false;
	}
	
	// save data and store new buffer index
	buffer[tmptail] = value;
	tail = tmptail;
	
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
void
xpcc::BoundedDeque<T, N, S>::popBack() {
	// calculate and store buffer index
	S tmphead = head - 1;
	if (tmphead >= N) {
		tmphead = N - 1;
	}
	head = tmphead;
}

// ----------------------------------------------------------------------------
template<typename T, int N, typename S>
void
xpcc::BoundedDeque<T, N, S>::popFront() {
	// calculate and store buffer index
	S tmptail = tail + 1;
	if (tmptail >= N) {
		tmptail = 0;
	}
	tail = tmptail;
}

/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
xpcc_always_inline bool
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
xpcc_always_inline bool
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
xpcc_always_inline typename xpcc::atomic::Queue<T, N>::Size
xpcc::atomic::Queue<T, N>::getMaxSize() const
{
	return N;
}

template<typename T, std::size_t N>
xpcc_always_inline const T&
xpcc::atomic::Queue<T, N>::get() const
{
	return this->buffer[this->tail];
}

template<typename T, std::size_t N>
xpcc_always_inline bool
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
xpcc_always_inline void
xpcc::atomic::Queue<T, N>::pop()
{
	Index tmptail = this->tail + 1;
	if (tmptail >= (N+1)) {
		tmptail = 0;
	}
	this->tail = tmptail;
}

#endif	// XPCC_ATOMIC__QUEUE_IMPL_HPP

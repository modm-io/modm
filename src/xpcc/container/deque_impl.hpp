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

#ifndef	XPCC__DEQUE_HPP
	#error	"Don't include this file directly use 'container/deque.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template<typename T, int N>
xpcc::BoundedDeque<T, N>::BoundedDeque() : 
	head(0), tail(1), size(0)
{
}

// ----------------------------------------------------------------------------

template<typename T, int N>
bool
xpcc::BoundedDeque<T, N>::isEmpty() const
{
	return (size == 0);
}

template<typename T, int N>
bool
xpcc::BoundedDeque<T, N>::isFull() const
{
	return (size == N);
}

template<typename T, int N>
typename xpcc::BoundedDeque<T, N>::Size
xpcc::BoundedDeque<T, N>::getSize() const
{
	return size;
}

template<typename T, int N>
typename xpcc::BoundedDeque<T, N>::Size
xpcc::BoundedDeque<T, N>::getMaxSize() const
{
	return N;
}

// ----------------------------------------------------------------------------

template<typename T, int N>
void
xpcc::BoundedDeque<T, N>::clear()
{
	head = 0;
	tail = 1;
	size = 0;
}

// ----------------------------------------------------------------------------

template<typename T, int N>
T&
xpcc::BoundedDeque<T, N>::front()
{
	return buffer[tail];
}


template<typename T, int N>
const T&
xpcc::BoundedDeque<T, N>::front() const
{
	return buffer[tail];
}

// ----------------------------------------------------------------------------

template<typename T, int N>
T&
xpcc::BoundedDeque<T, N>::back()
{
	return buffer[head];
}

template<typename T, int N>
const T&
xpcc::BoundedDeque<T, N>::back() const
{
	return buffer[head];
}

// ----------------------------------------------------------------------------

template<typename T, int N>
bool
xpcc::BoundedDeque<T, N>::pushBack(const T& value)
{
	if (this->isFull()) {
		return false;
	}
	
	head++;
	if (head >= N) {
		head = 0;
	}
	
	buffer[head] = value;
	size++;
	return true;
}

template<typename T, int N>
void
xpcc::BoundedDeque<T, N>::popBack()
{
	if (head == 0) {
		head = N - 1;
	}
	else {
		head--;
	}
	size--;
}

// ----------------------------------------------------------------------------

template<typename T, int N>
bool
xpcc::BoundedDeque<T, N>::pushFront(const T& value)
{
	if (this->isFull()) {
		return false;
	}
	
	if (tail == 0) {
		tail = N - 1;
	}
	else {
		tail--;
	}
	
	buffer[tail] = value;
	size++;
	return true;
}

template<typename T, int N>
void
xpcc::BoundedDeque<T, N>::popFront()
{
	tail++;
	if (tail >= N) {
		tail = 0;
	}
	size--;
}

// ----------------------------------------------------------------------------

template<typename T, int N>
xpcc::BoundedDeque<T, N>::const_iterator::const_iterator() :
	index(0), parent(0), count(0)
{
}

template<typename T, int N>
xpcc::BoundedDeque<T, N>::const_iterator::const_iterator(const const_iterator& other) :
	index(other.index), parent(other.parent), count(other.count)
{
}

template<typename T, int N>
typename xpcc::BoundedDeque<T, N>::const_iterator&
xpcc::BoundedDeque<T, N>::const_iterator::operator = (const const_iterator& other)
{
	index = other.index;
	count = other.count;
	parent = other.parent;
	
	return *this;
}

template<typename T, int N>
typename xpcc::BoundedDeque<T, N>::const_iterator&
xpcc::BoundedDeque<T, N>::const_iterator::operator ++ ()
{
	count++;
	if (count >= parent->size) {
		index = N;
	}
	else {
		index++;
		if (index >= N) {
			index = 0;
		}
	}
	return *this;
}

template<typename T, int N>
typename xpcc::BoundedDeque<T, N>::const_iterator&
xpcc::BoundedDeque<T, N>::const_iterator::operator -- ()
{
	if (count == 0) {
		index = N;
	}
	else {
		count--;
		if (index == 0)	{
			index = N - 1;
		}
		else {
			index--;
		}
	}
	return *this;
}

template<typename T, int N>
bool
xpcc::BoundedDeque<T, N>::const_iterator::operator == (const const_iterator& other) const
{
	return (index == other.index);
}

template<typename T, int N>
bool
xpcc::BoundedDeque<T, N>::const_iterator::operator != (const const_iterator& other) const
{
	return (index != other.index);
}

template<typename T, int N>
const T&
xpcc::BoundedDeque<T, N>::const_iterator::operator * () const
{
	return parent->buffer[index];
}

template<typename T, int N>
const T*
xpcc::BoundedDeque<T, N>::const_iterator::operator -> () const
{
	return &parent->buffer[index];
}

// ----------------------------------------------------------------------------

template<typename T, int N>
typename xpcc::BoundedDeque<T, N>::const_iterator
xpcc::BoundedDeque<T, N>::begin() const
{
	const_iterator it;
	
	it.index = tail;
	it.parent = this;
	
	return it;
}

template<typename T, int N>
typename xpcc::BoundedDeque<T, N>::const_iterator
xpcc::BoundedDeque<T, N>::end() const
{
	const_iterator it;
	
	it.index = N;
	it.parent = this;
	
	return it;
}

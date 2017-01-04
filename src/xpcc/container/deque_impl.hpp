/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_DEQUE_HPP
	#error	"Don't include this file directly use 'container/deque.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template<typename T, std::size_t N>
xpcc::BoundedDeque<T, N>::BoundedDeque() : 
	head(0), tail(1), size(0)
{
	static_assert(N > 0, "size = 0 is not allowed");
}

// ----------------------------------------------------------------------------

template<typename T, std::size_t N>
bool
xpcc::BoundedDeque<T, N>::isEmpty() const
{
	return (this->size == 0);
}

template<typename T, std::size_t N>
bool
xpcc::BoundedDeque<T, N>::isFull() const
{
	return (this->size == N);
}

template<typename T, std::size_t N>
typename xpcc::BoundedDeque<T, N>::Size
xpcc::BoundedDeque<T, N>::getSize() const
{
	return this->size;
}

template<typename T, std::size_t N>
typename xpcc::BoundedDeque<T, N>::Size
xpcc::BoundedDeque<T, N>::getMaxSize() const
{
	return N;
}

// ----------------------------------------------------------------------------

template<typename T, std::size_t N>
void
xpcc::BoundedDeque<T, N>::clear()
{
	this->head = 0;
	this->tail = 1;
	this->size = 0;
}

// ----------------------------------------------------------------------------

template<typename T, std::size_t N>
T&
xpcc::BoundedDeque<T, N>::getFront()
{
	return this->buffer[this->tail];
}


template<typename T, std::size_t N>
const T&
xpcc::BoundedDeque<T, N>::getFront() const
{
	return this->buffer[this->tail];
}

// ----------------------------------------------------------------------------

template<typename T, std::size_t N>
T&
xpcc::BoundedDeque<T, N>::getBack()
{
	return this->buffer[this->head];
}

template<typename T, std::size_t N>
const T&
xpcc::BoundedDeque<T, N>::getBack() const
{
	return this->buffer[this->head];
}

// ----------------------------------------------------------------------------

template<typename T, std::size_t N>
bool
xpcc::BoundedDeque<T, N>::append(const T& value)
{
	if (this->isFull()) {
		return false;
	}
	
	if (this->head >= (N - 1)) {
		this->head = 0;
	}
	else {
		this->head++;
	}
	
	this->buffer[this->head] = value;
	this->size++;
	return true;
}

template<typename T, std::size_t N>
void
xpcc::BoundedDeque<T, N>::removeBack()
{
	if (this->head == 0) {
		this->head = N - 1;
	}
	else {
		this->head--;
	}
	this->size--;
}

// ----------------------------------------------------------------------------

template<typename T, std::size_t N>
bool
xpcc::BoundedDeque<T, N>::prepend(const T& value)
{
	if (this->isFull()) {
		return false;
	}
	
	if (this->tail == 0) {
		this->tail = N - 1;
	}
	else {
		this->tail--;
	}
	
	this->buffer[this->tail] = value;
	this->size++;
	return true;
}

template<typename T, std::size_t N>
void
xpcc::BoundedDeque<T, N>::removeFront()
{
	if (this->tail >= (N - 1)) {
		this->tail = 0;
	}
	else {
		this->tail++;
	}
	
	this->size--;
}

// ----------------------------------------------------------------------------

template<typename T, std::size_t N>
xpcc::BoundedDeque<T, N>::const_iterator::const_iterator() :
	index(0), parent(0), count(0)
{
}

template<typename T, std::size_t N>
xpcc::BoundedDeque<T, N>::const_iterator::const_iterator(Index index,
		const BoundedDeque * parent) :
	index(index), parent(parent), count(0)
{
}

template<typename T, std::size_t N>
xpcc::BoundedDeque<T, N>::const_iterator::const_iterator(const const_iterator& other) :
	index(other.index), parent(other.parent), count(other.count)
{
}

template<typename T, std::size_t N>
typename xpcc::BoundedDeque<T, N>::const_iterator&
xpcc::BoundedDeque<T, N>::const_iterator::operator = (const const_iterator& other)
{
	this->index = other.index;
	this->count = other.count;
	this->parent = other.parent;
	
	return *this;
}

template<typename T, std::size_t N>
typename xpcc::BoundedDeque<T, N>::const_iterator&
xpcc::BoundedDeque<T, N>::const_iterator::operator ++ ()
{
	this->count++;
	if (this->count >= parent->size) {
		this->index = N;
	}
	else {
		this->index++;
		if (this->index >= N) {
			this->index = 0;
		}
	}
	return *this;
}

template<typename T, std::size_t N>
typename xpcc::BoundedDeque<T, N>::const_iterator&
xpcc::BoundedDeque<T, N>::const_iterator::operator -- ()
{
	if (this->count == 0) {
		this->index = N;
	}
	else {
		this->count--;
		if (this->index == 0)	{
			this->index = N - 1;
		}
		else {
			this->index--;
		}
	}
	return *this;
}

template<typename T, std::size_t N>
bool
xpcc::BoundedDeque<T, N>::const_iterator::operator == (const const_iterator& other) const
{
	return (this->index == other.index);
}

template<typename T, std::size_t N>
bool
xpcc::BoundedDeque<T, N>::const_iterator::operator != (const const_iterator& other) const
{
	return (this->index != other.index);
}

template<typename T, std::size_t N>
const T&
xpcc::BoundedDeque<T, N>::const_iterator::operator * () const
{
	return this->parent->buffer[index];
}

template<typename T, std::size_t N>
const T*
xpcc::BoundedDeque<T, N>::const_iterator::operator -> () const
{
	return &this->parent->buffer[index];
}

// ----------------------------------------------------------------------------

template<typename T, std::size_t N>
typename xpcc::BoundedDeque<T, N>::const_iterator
xpcc::BoundedDeque<T, N>::begin() const
{
	return const_iterator(this->tail, this);
}

template<typename T, std::size_t N>
typename xpcc::BoundedDeque<T, N>::const_iterator
xpcc::BoundedDeque<T, N>::end() const
{
	return const_iterator(N, this);
}

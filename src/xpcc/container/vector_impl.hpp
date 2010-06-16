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

#ifndef XPCC__VECTOR_HPP
	#error	"Don't include this file directly, use 'vector.hpp' instead"
#endif

// ----------------------------------------------------------------------------
template <typename T>
xpcc::Vector<T>::Vector() :
	size(0), remainingCapacity(0), buffer(0)
{
}

template <typename T>
xpcc::Vector<T>::Vector(SizeType n) :
	size(0), remainingCapacity(n), buffer(new T[n])
{
}

template <typename T>
xpcc::Vector<T>::Vector(SizeType n, const T& value) :
	size(n), remainingCapacity(0), buffer(new T[n])
{
	for (SizeType i = 0; i < n; ++i) {
		this->buffer[i] = value;
	}
}

template <typename T>
xpcc::Vector<T>::Vector(const Vector& other) :
	size(other.size), remainingCapacity(other.remainingCapacity),
	buffer(new T[other.size + other.remainingCapacity])
{
	for (SizeType i = 0; i < this->size; ++i) {
		this->buffer[i] = other.buffer[i];
	}
}

template <typename T>
xpcc::Vector<T>::~Vector()
{
	delete[] buffer;
}

template <typename T>
xpcc::Vector<T>&
xpcc::Vector<T>::operator = (const Vector& other)
{
	delete[] buffer;

	this->size = other.size;
	this->remainingCapacity = other.remainingCapacity;
	this->buffer = new T[this->size + this->remainingCapacity];

	for (SizeType i = 0; i < this->size; ++i) {
		this->buffer[i] = other.buffer[i];
	}
	return *this;
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::Vector<T>::reserve(SizeType n)
{
	if (n <= (this->remainingCapacity)) {
		// capacity is already big enough, nothing to do.
		return;
	}

	// allocate new memory
	this->relocate(this->size + n);
}

template <typename T>
T&
xpcc::Vector<T>::operator [](SizeType index)
{
	return this->buffer[index];
}

template <typename T>
const T&
xpcc::Vector<T>::operator [](SizeType index) const
{
	return this->buffer[index];
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::Vector<T>::append(const T& value)
{
	if (this->remainingCapacity == 0)
	{
		// allocate new memory if no more space is left
		SizeType n = this->size + (this->size + 1) / 2;
		if (n == 0) {
			n = 1;
		}
		this->relocate(n);
	}

	this->buffer[this->size] = value;
	++this->size;
}

template <typename T>
void
xpcc::Vector<T>::removeLast()
{
	--this->size;

	// call destructor
	this->buffer[this->size].~T();
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::Vector<T>::relocate(SizeType n)
{
	this->remainingCapacity = n - this->size;

	T* newBuffer = new T[n];
	for (SizeType i = 0; i < this->size; ++i) {
		newBuffer[i] = this->buffer[i];
	}
	delete[] this->buffer;

	this->buffer = newBuffer;
}

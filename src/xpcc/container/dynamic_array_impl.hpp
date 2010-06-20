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

#ifndef XPCC__DYNAMIC_ARRAY_HPP
	#error	"Don't include this file directly, use 'vector.hpp' instead"
#endif

// ----------------------------------------------------------------------------
template <typename T>
xpcc::DynamicArray<T>::DynamicArray() :
	size(0), capacity(0), values(0)
{
}

template <typename T>
xpcc::DynamicArray<T>::DynamicArray(SizeType n) :
	size(0), capacity(n), values(new T[n])
{
}

template <typename T>
xpcc::DynamicArray<T>::DynamicArray(SizeType n, const T& value) :
	size(n), capacity(n), values(new T[n])
{
	for (SizeType i = 0; i < n; ++i) {
		this->values[i] = value;
	}
}

template <typename T>
xpcc::DynamicArray<T>::DynamicArray(const DynamicArray& other) :
	size(other.size), capacity(other.capacity),
	values(new T[other.capacity])
{
	for (SizeType i = 0; i < this->size; ++i) {
		this->values[i] = other.values[i];
	}
}

template <typename T>
xpcc::DynamicArray<T>::~DynamicArray()
{
	delete[] this->values;
}

template <typename T>
xpcc::DynamicArray<T>&
xpcc::DynamicArray<T>::operator = (const DynamicArray& other)
{
	delete[] this->values;

	this->size = other.size;
	this->capacity = other.capacity;
	this->values = new T[this->capacity];

	for (SizeType i = 0; i < this->size; ++i) {
		this->values[i] = other.values[i];
	}
	return *this;
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::DynamicArray<T>::reserve(SizeType n)
{
	if (n <= (this->capacity - this->size)) {
		// capacity is already big enough, nothing to do.
		return;
	}

	// allocate new memory
	this->relocate(this->size + n);
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::DynamicArray<T>::append(const T& value)
{
	if (this->capacity == this->size)
	{
		// allocate new memory if no more space is left
		SizeType n = this->size + (this->size + 1) / 2;
		if (n == 0) {
			n = 1;
		}
		this->relocate(n);
	}

	this->values[this->size] = value;
	++this->size;
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::DynamicArray<T>::removeLast()
{
	--this->size;

	// call destructor
	this->values[this->size].~T();
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::DynamicArray<T>::relocate(SizeType n)
{
	this->capacity = n;
	
	T* newBuffer = new T[n];
	for (SizeType i = 0; i < this->size; ++i) {
		newBuffer[i] = this->values[i];
	}
	delete[] this->values;
	
	this->values = newBuffer;
}

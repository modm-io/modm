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

#ifndef XPCC__DYNAMIC_ARRAY_HPP
	#error	"Don't include this file directly, use 'vector.hpp' instead"
#endif

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::DynamicArray(const Allocator& alloc) :
	allocator(alloc),
	size(0), capacity(0), values(0)
{
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::DynamicArray(SizeType n, const Allocator& alloc) :
	allocator(alloc), size(0), capacity(n)
{
	this->values = this->allocator.allocate(n);
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::DynamicArray(SizeType n, const T& value, const Allocator& alloc) :
	allocator(alloc), size(n), capacity(n)
{
	this->values = this->allocator.allocate(n);
	for (SizeType i = 0; i < n; ++i) {
		allocator.construct(&this->values[i], value);
	}
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::DynamicArray(const DynamicArray& other) :
	allocator(other.allocator),
	size(other.size), capacity(other.capacity)
{
	this->values = allocator.allocate(other.capacity);
	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.construct(&this->values[i], other.values[i]);
	}
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::~DynamicArray()
{
	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.destroy(&this->values[i]);
	}
	this->allocator.deallocate(this->values);
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>&
xpcc::DynamicArray<T, Allocator>::operator = (const DynamicArray& other)
{
	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.destroy(&this->values[i]);
	}
	this->allocator.deallocate(this->values);
	
	this->allocator = other.allocator;
	this->size = other.size;
	this->capacity = other.capacity;
	this->values = this->allocator.allocate(this->capacity);

	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.construct(&this->values[i], other.values[i]);
	}
	return *this;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
xpcc::DynamicArray<T, Allocator>::reserve(SizeType n)
{
	if (n <= (this->capacity - this->size)) {
		// capacity is already big enough, nothing to do.
		return;
	}

	// allocate new memory
	this->relocate(this->size + n);
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
xpcc::DynamicArray<T, Allocator>::clear()
{
	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.destroy(&this->values[i]);
	}
	this->allocator.deallocate(this->values);
	this->values = 0;
	
	this->size = 0;
	this->capacity = 0;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
xpcc::DynamicArray<T, Allocator>::removeAll()
{
	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.destroy(&this->values[i]);
	}
	this->size = 0;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
xpcc::DynamicArray<T, Allocator>::append(const T& value)
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
	
	this->allocator.construct(&this->values[this->size], value);
	++this->size;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
xpcc::DynamicArray<T, Allocator>::removeBack()
{
	--this->size;

	// call destructor
	this->allocator.destroy(&this->values[this->size]);
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
xpcc::DynamicArray<T, Allocator>::relocate(SizeType n)
{
	this->capacity = n;
	
	T* newBuffer = allocator.allocate(n);
	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.construct(&newBuffer[i], this->values[i]);
		this->allocator.destroy(&this->values[i]);
	}
	this->allocator.deallocate(this->values);
	
	this->values = newBuffer;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::iterator 
xpcc::DynamicArray<T, Allocator>::begin()
{
	return iterator(this, 0);
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::iterator
xpcc::DynamicArray<T, Allocator>::end()
{
	return iterator(this, size);
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::iterator
xpcc::DynamicArray<T, Allocator>::find(const T& value)
{
	xpcc::DynamicArray<T, Allocator>::iterator iter = this->begin();

	for(; iter != this->end(); ++iter)
	{
		if((*iter) == value) {
			return iter;
		}
	}

	// return end() if value has not been found
	return iter;
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::const_iterator 
xpcc::DynamicArray<T, Allocator>::begin() const
{
	return const_iterator(this, 0);
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::const_iterator
xpcc::DynamicArray<T, Allocator>::end() const
{
	return const_iterator(this, size);
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::const_iterator
xpcc::DynamicArray<T, Allocator>::find(const T& value) const
{
	xpcc::DynamicArray<T, Allocator>::const_iterator iter = this->begin();

	for(; iter != this->end(); ++iter)
	{
		if((*iter) == value) {
			return iter;
		}
	}

	// return end() if value has not been found
	return iter;
}

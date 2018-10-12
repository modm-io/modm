/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DYNAMIC_ARRAY_HPP
	#error	"Don't include this file directly, use 'vector.hpp' instead"
#endif

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::DynamicArray(const Allocator& alloc) :
	allocator(alloc),
	size(0), capacity(0), values(0)
{
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::DynamicArray(SizeType n, const Allocator& alloc) :
	allocator(alloc), size(0), capacity(n)
{
	this->values = this->allocator.allocate(n);
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::DynamicArray(SizeType n, const T& value, const Allocator& alloc) :
	allocator(alloc), size(n), capacity(n)
{
	this->values = this->allocator.allocate(n);
	for (SizeType i = 0; i < n; ++i) {
		allocator.construct(&this->values[i], value);
	}
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::DynamicArray(std::initializer_list<T> init, const Allocator& alloc) :
	allocator(alloc), size(init.size()), capacity(init.size())
{
	this->values = this->allocator.allocate(init.size());
	std::size_t ii = 0;
	for (auto value : init) {
		allocator.construct(&this->values[ii], value);
		++ii;
	}
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::DynamicArray(const DynamicArray& other) :
	allocator(other.allocator),
	size(other.size), capacity(other.capacity)
{
	this->values = allocator.allocate(other.capacity);
	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.construct(&this->values[i], other.values[i]);
	}
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::~DynamicArray()
{
	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.destroy(&this->values[i]);
	}
	this->allocator.deallocate(this->values);
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>&
modm::DynamicArray<T, Allocator>::operator = (const DynamicArray& other)
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
modm::DynamicArray<T, Allocator>::reserve(SizeType n)
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
modm::DynamicArray<T, Allocator>::clear()
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
modm::DynamicArray<T, Allocator>::removeAll()
{
	for (SizeType i = 0; i < this->size; ++i) {
		this->allocator.destroy(&this->values[i]);
	}
	this->size = 0;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
modm::DynamicArray<T, Allocator>::append(const T& value)
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
modm::DynamicArray<T, Allocator>::removeBack()
{
	--this->size;

	// call destructor
	this->allocator.destroy(&this->values[this->size]);
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
modm::DynamicArray<T, Allocator>::relocate(SizeType n)
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
typename modm::DynamicArray<T, Allocator>::iterator
modm::DynamicArray<T, Allocator>::begin()
{
	return iterator(this, 0);
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::iterator
modm::DynamicArray<T, Allocator>::end()
{
	return iterator(this, size);
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::iterator
modm::DynamicArray<T, Allocator>::find(const T& value)
{
	modm::DynamicArray<T, Allocator>::iterator iter = this->begin();

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
typename modm::DynamicArray<T, Allocator>::const_iterator
modm::DynamicArray<T, Allocator>::begin() const
{
	return const_iterator(this, 0);
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::const_iterator
modm::DynamicArray<T, Allocator>::end() const
{
	return const_iterator(this, size);
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::const_iterator
modm::DynamicArray<T, Allocator>::find(const T& value) const
{
	modm::DynamicArray<T, Allocator>::const_iterator iter = this->begin();

	for(; iter != this->end(); ++iter)
	{
		if((*iter) == value) {
			return iter;
		}
	}

	// return end() if value has not been found
	return iter;
}

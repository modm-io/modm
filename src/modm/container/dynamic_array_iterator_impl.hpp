/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2014, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_DYNAMIC_ARRAY_HPP
	#error	"Don't include this file directly, use 'dynamic_array.hpp' instead"
#endif

// ----------------------------------------------------------------------------
// Iterators
// ----------------------------------------------------------------------------

// const iterator
template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::const_iterator::const_iterator() :
	parent(0),
	index(0)
{
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::const_iterator::const_iterator(
		const DynamicArray* inParent, SizeType inIndex) :
	parent(inParent),
	index(inIndex)
{
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::const_iterator::const_iterator(
		const iterator& other) :
	parent(other.parent),
	index(other.index)
{
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::const_iterator::const_iterator(
		const const_iterator& other) :
	parent(other.parent),
	index(other.index)
{
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::const_iterator&
modm::DynamicArray<T, Allocator>::const_iterator::operator = (
		const const_iterator& other)
{
	this->parent = other.parent;
	this->index = other.index;
	return *this;
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::const_iterator&
modm::DynamicArray<T, Allocator>::const_iterator::operator ++ ()
{
	++this->index;
	return *this;
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::const_iterator&
modm::DynamicArray<T, Allocator>::const_iterator::operator -- ()
{
	--this->index;
	return *this;
}

template <typename T, typename Allocator>
bool
modm::DynamicArray<T, Allocator>::const_iterator::operator == (
		const const_iterator& other) const
{
	return ((parent == other.parent) &&
			(index == other.index));
}

template <typename T, typename Allocator>
bool
modm::DynamicArray<T, Allocator>::const_iterator::operator != (
		const const_iterator& other) const
{
	return ((parent != other.parent) ||
			(index != other.index));
}

template <typename T, typename Allocator>
const T&
modm::DynamicArray<T, Allocator>::const_iterator::operator * () const
{
	return parent->values[index];
}

template <typename T, typename Allocator>
const T*
modm::DynamicArray<T, Allocator>::const_iterator::operator -> () const
{
	return &parent->values[index];
}

// ----------------------------------------------------------------------------
// iterator
template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::iterator::iterator() :
	parent(0),
	index(0)
{
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::iterator::iterator(
		DynamicArray* inParent, SizeType inIndex) :
	parent(inParent),
	index(inIndex)
{
}

template <typename T, typename Allocator>
modm::DynamicArray<T, Allocator>::iterator::iterator(const iterator& other) :
	parent(other.parent),
	index(other.index)
{
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::iterator&
modm::DynamicArray<T, Allocator>::iterator::operator = (const iterator& other)
{
	this->parent = other.parent;
	this->index = other.index;
	return *this;
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::iterator&
modm::DynamicArray<T, Allocator>::iterator::operator ++ ()
{
	++index;
	return *this;
}

template <typename T, typename Allocator>
typename modm::DynamicArray<T, Allocator>::iterator&
modm::DynamicArray<T, Allocator>::iterator::operator -- ()
{
	--index;
	return *this;
}

template <typename T, typename Allocator>
bool
modm::DynamicArray<T, Allocator>::iterator::operator == (
		const iterator& other) const
{
	return ((parent == other.parent) &&
			(index == other.index));
}

template <typename T, typename Allocator>
bool
modm::DynamicArray<T, Allocator>::iterator::operator != (
		const iterator& other) const
{
	return ((parent != other.parent) ||
			(index != other.index));
}

template <typename T, typename Allocator>
bool
modm::DynamicArray<T, Allocator>::iterator::operator < (
		const iterator& other) const
{
	return ((parent == other.parent) &&
			(index < other.index));
}

template <typename T, typename Allocator>
bool
modm::DynamicArray<T, Allocator>::iterator::operator > (
		const iterator& other) const
{
	return ((parent == other.parent) &&
			(index > other.index));
}


template <typename T, typename Allocator>
T&
modm::DynamicArray<T, Allocator>::iterator::operator * ()
{
	return parent->values[index];
}

template <typename T, typename Allocator>
T*
modm::DynamicArray<T, Allocator>::iterator::operator -> ()
{
	return &parent->values[index];
}

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
 * $Id: dynamic_array_iterator_impl.hpp 611 2011-09-29 13:09:43Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__DYNAMIC_ARRAY_HPP
	#error	"Don't include this file directly, use 'dynamic_array.hpp' instead"
#endif

// ----------------------------------------------------------------------------
// Iterators
// ----------------------------------------------------------------------------

// const iterator
template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::const_iterator::const_iterator() :
	parent(0),
	index(0)
{
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::const_iterator::const_iterator(
		const DynamicArray* inParent, SizeType inIndex) :
	parent(inParent),
	index(inIndex)
{
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::const_iterator::const_iterator(
		const iterator& other) :
	parent(other.parent),
	index(other.index)
{
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::const_iterator::const_iterator(
		const const_iterator& other) :
	parent(other.parent),
	index(other.index)
{
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::const_iterator&
xpcc::DynamicArray<T, Allocator>::const_iterator::operator = (
		const const_iterator& other)
{
	this->parent = other.parent;
	this->index = other.index;
	return *this;
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::const_iterator&
xpcc::DynamicArray<T, Allocator>::const_iterator::operator ++ ()
{
	++this->index;
	return *this;
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::const_iterator&
xpcc::DynamicArray<T, Allocator>::const_iterator::operator -- ()
{
	--this->index;
	return *this;
}

template <typename T, typename Allocator>
bool
xpcc::DynamicArray<T, Allocator>::const_iterator::operator == (
		const const_iterator& other) const
{
	return ((parent == other.parent) &&
			(index == other.index));
}

template <typename T, typename Allocator>
bool
xpcc::DynamicArray<T, Allocator>::const_iterator::operator != (
		const const_iterator& other) const
{
	return ((parent != other.parent) ||
			(index != other.index));
}

template <typename T, typename Allocator>
const T&
xpcc::DynamicArray<T, Allocator>::const_iterator::operator * () const
{
	return parent->values[index];
}

template <typename T, typename Allocator>
const T* 
xpcc::DynamicArray<T, Allocator>::const_iterator::operator -> () const
{
	return &parent->values[index];
}

// ----------------------------------------------------------------------------
// iterator
template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::iterator::iterator() :
	parent(0),
	index(0)
{
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::iterator::iterator(
		DynamicArray* inParent, SizeType inIndex) :
	parent(inParent),
	index(inIndex)
{
}

template <typename T, typename Allocator>
xpcc::DynamicArray<T, Allocator>::iterator::iterator(const iterator& other) :
	parent(other.parent),
	index(other.index)
{
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::iterator&
xpcc::DynamicArray<T, Allocator>::iterator::operator = (const iterator& other)
{
	this->parent = other.parent;
	this->index = other.index;
	return *this;
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::iterator&
xpcc::DynamicArray<T, Allocator>::iterator::operator ++ ()
{
	++index;
	return *this;
}

template <typename T, typename Allocator>
typename xpcc::DynamicArray<T, Allocator>::iterator&
xpcc::DynamicArray<T, Allocator>::iterator::operator -- ()
{
	--index;
	return *this;
}

template <typename T, typename Allocator>
bool
xpcc::DynamicArray<T, Allocator>::iterator::operator == (
		const iterator& other) const
{
	return ((parent == other.parent) &&
			(index == other.index));
}

template <typename T, typename Allocator>
bool
xpcc::DynamicArray<T, Allocator>::iterator::operator != (
		const iterator& other) const
{
	return ((parent != other.parent) ||
			(index != other.index));
}

template <typename T, typename Allocator>
T&
xpcc::DynamicArray<T, Allocator>::iterator::operator * ()
{
	return parent->values[index];
}

template <typename T, typename Allocator>
T* 
xpcc::DynamicArray<T, Allocator>::iterator::operator -> ()
{
	return &parent->values[index];
}

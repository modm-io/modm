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

#ifndef	XPCC__LINKED_LIST_HPP
	#error	"Don't include this file directly, use 'linked_list.hpp' instead"
#endif

// ----------------------------------------------------------------------------
// Iterators
// ----------------------------------------------------------------------------

// const iterator
template <typename T, typename Allocator>
xpcc::LinkedList<T, Allocator>::const_iterator::const_iterator() :
	node(0)
{
}

template <typename T, typename Allocator>
xpcc::LinkedList<T, Allocator>::const_iterator::const_iterator(
		Node* node) :
	node(node)
{
}

template <typename T, typename Allocator>
xpcc::LinkedList<T, Allocator>::const_iterator::const_iterator(
		const iterator& other) :
	node(other.node)
{
}

template <typename T, typename Allocator>
xpcc::LinkedList<T, Allocator>::const_iterator::const_iterator(
		const const_iterator& other) :
	node(other.node)
{
}

template <typename T, typename Allocator>
typename xpcc::LinkedList<T, Allocator>::const_iterator&
xpcc::LinkedList<T, Allocator>::const_iterator::operator = (const const_iterator& other)
{
	this->node = other.node;
	return *this;
}

template <typename T, typename Allocator>
typename xpcc::LinkedList<T, Allocator>::const_iterator&
xpcc::LinkedList<T, Allocator>::const_iterator::operator ++ ()
{
	this->node = this->node->next;
	return *this;
}

template <typename T, typename Allocator>
bool
xpcc::LinkedList<T, Allocator>::const_iterator::operator == (
		const const_iterator& other) const
{
	return (node == other.node);
}

template <typename T, typename Allocator>
bool
xpcc::LinkedList<T, Allocator>::const_iterator::operator != (
		const const_iterator& other) const
{
	return (node != other.node);
}

template <typename T, typename Allocator>
const T&
xpcc::LinkedList<T, Allocator>::const_iterator::operator * () const
{
	return this->node->value;
}

template <typename T, typename Allocator>
const T* 
xpcc::LinkedList<T, Allocator>::const_iterator::operator -> () const
{
	return &this->node->value;
}


template <typename T, typename Allocator>
typename xpcc::LinkedList<T, Allocator>::const_iterator 
xpcc::LinkedList<T, Allocator>::begin() const
{
	return const_iterator(this->front);
}

template <typename T, typename Allocator>
typename xpcc::LinkedList<T, Allocator>::const_iterator
xpcc::LinkedList<T, Allocator>::end() const
{
	return const_iterator(0);
}

// iterator
template <typename T, typename Allocator>
xpcc::LinkedList<T, Allocator>::iterator::iterator() :
	node(0)
{
}

template <typename T, typename Allocator>
xpcc::LinkedList<T, Allocator>::iterator::iterator(Node* node) :
	node(node)
{
}

template <typename T, typename Allocator>
xpcc::LinkedList<T, Allocator>::iterator::iterator(
		const iterator& other) :
	node(other.node)
{
}

template <typename T, typename Allocator>
typename xpcc::LinkedList<T, Allocator>::iterator&
xpcc::LinkedList<T, Allocator>::iterator::operator = (const iterator& other)
{
	this->node = other.node;
	return *this;
}

template <typename T, typename Allocator>
typename xpcc::LinkedList<T, Allocator>::iterator&
xpcc::LinkedList<T, Allocator>::iterator::operator ++ ()
{
	this->node = this->node->next;
	return *this;
}

template <typename T, typename Allocator>
bool
xpcc::LinkedList<T, Allocator>::iterator::operator == (
		const iterator& other) const
{
	return (node == other.node);
}

template <typename T, typename Allocator>
bool
xpcc::LinkedList<T, Allocator>::iterator::operator != (
		const iterator& other) const
{
	return (node != other.node);
}

template <typename T, typename Allocator>
T&
xpcc::LinkedList<T, Allocator>::iterator::operator * ()
{
	return this->node->value;
}

template <typename T, typename Allocator>
T* 
xpcc::LinkedList<T, Allocator>::iterator::operator -> ()
{
	return &this->node->value;
}


template <typename T, typename Allocator>
typename xpcc::LinkedList<T, Allocator>::iterator 
xpcc::LinkedList<T, Allocator>::begin()
{
	return iterator(this->front);
}

template <typename T, typename Allocator>
typename xpcc::LinkedList<T, Allocator>::iterator
xpcc::LinkedList<T, Allocator>::end()
{
	return iterator(0);
}

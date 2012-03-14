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
 * $hello please remove me$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__DOUBLY_LINKED_LIST_HPP
	#error	"Don't include this file directly, use 'doubly_linked_list.hpp' instead"
#endif

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
xpcc::DoublyLinkedList<T, Allocator>::DoublyLinkedList(const Allocator& allocator) :
	nodeAllocator(allocator), front(0), back(0)
{
}

template <typename T, typename Allocator>
xpcc::DoublyLinkedList<T, Allocator>::~DoublyLinkedList()
{
	while (this->front != 0)
	{
		Node *node = this->front;
		this->front = this->front->next;
		
		Allocator::destroy(&node->value);
		this->nodeAllocator.deallocate(node);
	}
}

template <typename T, typename Allocator>
bool
xpcc::DoublyLinkedList<T, Allocator>::isEmpty() const
{
	return (this->front == 0);
}

template <typename T, typename Allocator>
std::size_t
xpcc::DoublyLinkedList<T, Allocator>::getSize() const
{
	std::size_t count = 0;
	for (const_iterator it = this->begin(); it != this->end(); ++it) {
		count++;
	}
	return count;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
bool
xpcc::DoublyLinkedList<T, Allocator>::prepend(const T& value)
{
	// allocate memory for the new node and copy the value into it
	Node *node = this->nodeAllocator.allocate(1);
	Allocator::construct(&node->value, value);
	
	// hook the node into the list
	node->next = this->front;
	node->previous = 0;
	
	if (this->front == 0)
	{
		// node it the first entry
		this->back = node;
	}
	else {
		this->front->previous = node;
	}
	this->front = node;
	
	return true;
}

template <typename T, typename Allocator>
void
xpcc::DoublyLinkedList<T, Allocator>::append(const T& value)
{
	// allocate memory for the new node and copy the value into it
	Node *node = this->nodeAllocator.allocate(1);
	Allocator::construct(&node->value, value);
	
	// hook the node into the list
	node->next = 0;
	node->previous = this->back;
	
	if (this->back == 0)
	{
		// first entry in the list
		this->front = node;
	}
	else {
		this->back->next = node;
	}
	this->back = node;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
xpcc::DoublyLinkedList<T, Allocator>::removeFront()
{
	// remove node from the list
	Node *node = this->front;
	
	if (this->front->next == 0)
	{
		// last entry in the list
		this->front = 0;
		this->back = 0;
	}
	else {
		this->front = this->front->next;
		this->front->previous = 0;
	}
	
	// call destructor and free memory
	Allocator::destroy(&node->value);
	this->nodeAllocator.deallocate(node);
}

template <typename T, typename Allocator>
void
xpcc::DoublyLinkedList<T, Allocator>::removeBack()
{
	// remove node from the list
	Node *node = this->back;
	
	if (this->back->previous == 0)
	{
		// last entry in the list
		this->front = 0;
		this->back = 0;
	}
	else {
		this->back = this->back->previous;
		this->back->next = 0;
	}
	
	// call destructor and free memory
	Allocator::destroy(&node->value);
	this->nodeAllocator.deallocate(node);
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
inline const T&
xpcc::DoublyLinkedList<T, Allocator>::getFront() const
{
	return this->front->value;
}

template <typename T, typename Allocator>
inline const T&
xpcc::DoublyLinkedList<T, Allocator>::getBack() const
{
	return this->back->value;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
typename xpcc::DoublyLinkedList<T, Allocator>::iterator 
xpcc::DoublyLinkedList<T, Allocator>::begin()
{
	return iterator(this->front);
}

template <typename T, typename Allocator>
typename xpcc::DoublyLinkedList<T, Allocator>::iterator
xpcc::DoublyLinkedList<T, Allocator>::end()
{
	return iterator(0);
}

template <typename T, typename Allocator>
typename xpcc::DoublyLinkedList<T, Allocator>::const_iterator 
xpcc::DoublyLinkedList<T, Allocator>::begin() const
{
	return const_iterator(this->front);
}

template <typename T, typename Allocator>
typename xpcc::DoublyLinkedList<T, Allocator>::const_iterator
xpcc::DoublyLinkedList<T, Allocator>::end() const
{
	return const_iterator(0);
}

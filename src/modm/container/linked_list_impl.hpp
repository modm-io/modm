/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_LINKED_LIST_HPP
	#error	"Don't include this file directly, use 'linked_list.hpp' instead"
#endif

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
modm::LinkedList<T, Allocator>::LinkedList(const Allocator& allocator) :
	nodeAllocator(allocator), front(0), back(0)
{
}

template <typename T, typename Allocator>
modm::LinkedList<T, Allocator>::~LinkedList()
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
modm::LinkedList<T, Allocator>::isEmpty() const
{
	return (this->front == 0);
}

template <typename T, typename Allocator>
std::size_t
modm::LinkedList<T, Allocator>::getSize() const
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
modm::LinkedList<T, Allocator>::prepend(const T& value)
{
	// allocate memory for the new node and copy the value into it
	Node *node = this->nodeAllocator.allocate(1);
	Allocator::construct(&node->value, value);

	// hook the node into the list
	node->next = this->front;
	this->front = node;

	if (this->back == 0) {
		// first entry in the list
		this->back = node;
	}

	return true;
}

template <typename T, typename Allocator>
bool
modm::LinkedList<T, Allocator>::append(const T& value)
{
	// allocate memory for the new node and copy the value into it
	Node *node = this->nodeAllocator.allocate(1);
	Allocator::construct(&node->value, value);

	// hook the node into the list
	node->next = 0;
	if (this->front == 0)
	{
		// first entry in the list
		this->front = node;
	}
	else {
		this->back->next = node;
	}
	this->back = node;

	return true;
}

template <typename T, typename Allocator>
bool
modm::LinkedList<T, Allocator>::insert(const_iterator pos, const T& value)
{
	// if pos is the `end` iterator
	if(pos.node == nullptr) {
		return this->append(value);
	}

	// allocate memory for the new node and copy the value into it
	Node *node = this->nodeAllocator.allocate(1);
	Allocator::construct(&node->value, value);

	// hook the node into the list
	node->next = pos.node->next;
	pos.node->next = node;
	if(this->back == pos.node) {
		this->back = node;
	}

	return true;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
modm::LinkedList<T, Allocator>::removeFront()
{
	// remove node from the list
	Node *node = this->front;
	if (this->front->next == 0)
	{
		// last entry in the list
		this->back = 0;
		this->front = 0;
	}
	else {
		this->front = this->front->next;
	}

	// call destructor and free memory
	Allocator::destroy(&node->value);
	this->nodeAllocator.deallocate(node);
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
inline const T&
modm::LinkedList<T, Allocator>::getFront() const
{
	return this->front->value;
}

template <typename T, typename Allocator>
inline T&
modm::LinkedList<T, Allocator>::getFront()
{
	return this->front->value;
}

template <typename T, typename Allocator>
inline const T&
modm::LinkedList<T, Allocator>::getBack() const
{
	return this->back->value;
}

template <typename T, typename Allocator>
inline T&
modm::LinkedList<T, Allocator>::getBack()
{
	return this->back->value;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
void
modm::LinkedList<T, Allocator>::removeAll()
{
	while (!this->isEmpty()) {
		this->removeFront();
	}
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
typename modm::LinkedList<T, Allocator>::iterator
modm::LinkedList<T, Allocator>::remove(const iterator& iter)
{
	if (this->isEmpty()) {
		return this->begin();
	}
	else if (iter.node == this->front)
	{
		this->removeFront();
		return this->begin();
	}

	Node *node = this->front;
	while (node->next != 0)
	{
		if (node->next == iter.node)
		{
			if (iter.node == this->back) {
				this->back = node;
			}
			node->next = iter.node->next;

			// call destructor and free memory
			Allocator::destroy(&iter.node->value);
			this->nodeAllocator.deallocate(iter.node);

			return iterator(node->next);
		}
		node = node->next;
	}

	return iter;
}

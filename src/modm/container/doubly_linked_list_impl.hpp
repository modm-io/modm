/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, 2011, Thorsten Lajewski
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

#ifndef	MODM_DOUBLY_LINKED_LIST_HPP
	#error	"Don't include this file directly, use 'doubly_linked_list.hpp' instead"
#endif

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
modm::DoublyLinkedList<T, Allocator>::DoublyLinkedList(const Allocator& allocator) :
	nodeAllocator(allocator), front(0), back(0)
{
}

template <typename T, typename Allocator>
modm::DoublyLinkedList<T, Allocator>::~DoublyLinkedList()
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
modm::DoublyLinkedList<T, Allocator>::isEmpty() const
{
	return (this->front == 0);
}

template <typename T, typename Allocator>
std::size_t
modm::DoublyLinkedList<T, Allocator>::getSize() const
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
modm::DoublyLinkedList<T, Allocator>::prepend(const T& value)
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
modm::DoublyLinkedList<T, Allocator>::append(const T& value)
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
modm::DoublyLinkedList<T, Allocator>::removeFront()
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
modm::DoublyLinkedList<T, Allocator>::removeBack()
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
modm::DoublyLinkedList<T, Allocator>::getFront() const
{
	return this->front->value;
}

template <typename T, typename Allocator>
inline const T&
modm::DoublyLinkedList<T, Allocator>::getBack() const
{
	return this->back->value;
}

// ----------------------------------------------------------------------------
template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::iterator
modm::DoublyLinkedList<T, Allocator>::begin()
{
	return iterator(this->front);
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::iterator
modm::DoublyLinkedList<T, Allocator>::end()
{
	return iterator(0);
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::const_iterator
modm::DoublyLinkedList<T, Allocator>::begin() const
{
	return const_iterator(this->front);
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::const_iterator
modm::DoublyLinkedList<T, Allocator>::end() const
{
	return const_iterator(0);
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::iterator
modm::DoublyLinkedList<T, Allocator>::erase(iterator position)
{

	if(position.node->previous == 0) {
		this->removeFront();
		return this->begin();
	}

	if(position.node->next == 0) {
		this->removeBack();
		return this->end();
	}

	position.node->previous->next = position.node->next;
	position.node->next->previous = position.node->previous;

	Node* next = position.node->next;

	Allocator::destroy(&(position.node->value));
	this->nodeAllocator.deallocate(position.node);

	return iterator(next);

}

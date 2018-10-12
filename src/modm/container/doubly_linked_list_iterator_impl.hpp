/*
 * Copyright (c) 2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
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
// Iterators
// ----------------------------------------------------------------------------

// const iterator
template <typename T, typename Allocator>
modm::DoublyLinkedList<T, Allocator>::const_iterator::const_iterator() :
	node(0)
{
}

template <typename T, typename Allocator>
modm::DoublyLinkedList<T, Allocator>::const_iterator::const_iterator(
		const Node* node) :
	node(node)
{
}

template <typename T, typename Allocator>
modm::DoublyLinkedList<T, Allocator>::const_iterator::const_iterator(
		const iterator& other) :
	node(other.node)
{
}

template <typename T, typename Allocator>
modm::DoublyLinkedList<T, Allocator>::const_iterator::const_iterator(
		const const_iterator& other) :
	node(other.node)
{
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::const_iterator&
modm::DoublyLinkedList<T, Allocator>::const_iterator::operator = (
		const const_iterator& other)
{
	this->node = other.node;
	return *this;
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::const_iterator&
modm::DoublyLinkedList<T, Allocator>::const_iterator::operator ++ ()
{
	this->node = this->node->next;
	return *this;
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::const_iterator&
modm::DoublyLinkedList<T, Allocator>::const_iterator::operator -- ()
{
	this->node = this->node->previous;
	return *this;
}

template <typename T, typename Allocator>
bool
modm::DoublyLinkedList<T, Allocator>::const_iterator::operator == (
		const const_iterator& other) const
{
	return (node == other.node);
}

template <typename T, typename Allocator>
bool
modm::DoublyLinkedList<T, Allocator>::const_iterator::operator != (
		const const_iterator& other) const
{
	return (node != other.node);
}

template <typename T, typename Allocator>
const T&
modm::DoublyLinkedList<T, Allocator>::const_iterator::operator * () const
{
	return this->node->value;
}

template <typename T, typename Allocator>
const T*
modm::DoublyLinkedList<T, Allocator>::const_iterator::operator -> () const
{
	return &this->node->value;
}


// iterator
template <typename T, typename Allocator>
modm::DoublyLinkedList<T, Allocator>::iterator::iterator() :
	node(0)
{
}

template <typename T, typename Allocator>
modm::DoublyLinkedList<T, Allocator>::iterator::iterator(Node* node) :
	node(node)
{
}

template <typename T, typename Allocator>
modm::DoublyLinkedList<T, Allocator>::iterator::iterator(
		const iterator& other) :
	node(other.node)
{
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::iterator&
modm::DoublyLinkedList<T, Allocator>::iterator::operator = (const iterator& other)
{
	this->node = other.node;
	return *this;
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::iterator&
modm::DoublyLinkedList<T, Allocator>::iterator::operator ++ ()
{
	this->node = this->node->next;
	return *this;
}

template <typename T, typename Allocator>
typename modm::DoublyLinkedList<T, Allocator>::iterator&
modm::DoublyLinkedList<T, Allocator>::iterator::operator -- ()
{
	this->node = this->node->previous;
	return *this;
}

template <typename T, typename Allocator>
bool
modm::DoublyLinkedList<T, Allocator>::iterator::operator == (
		const iterator& other) const
{
	return (node == other.node);
}

template <typename T, typename Allocator>
bool
modm::DoublyLinkedList<T, Allocator>::iterator::operator != (
		const iterator& other) const
{
	return (node != other.node);
}

template <typename T, typename Allocator>
T&
modm::DoublyLinkedList<T, Allocator>::iterator::operator * ()
{
	return this->node->value;
}

template <typename T, typename Allocator>
T*
modm::DoublyLinkedList<T, Allocator>::iterator::operator -> ()
{
	return &this->node->value;
}

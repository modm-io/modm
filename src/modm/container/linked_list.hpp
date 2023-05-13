/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Thorsten Lajewski
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_LINKED_LIST_HPP
#define	MODM_LINKED_LIST_HPP

#include <modm/container/doubly_linked_list.hpp>

namespace modm
{
	/**
	 * \brief	Singly-linked list
	 *
	 *
	 * \todo	implementation
	 *
	 * \tparam	T			Type of list entries
	 * \tparam	Allocator	Allocator used for memory allocation. See
	 * 						classes from modm::allocator namespace.
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_container
	 *
	 * \todo TODO: implementated as doubly-linked list
	 * 		 std::forward_list does not save both front and back pointers.
	 * 		 This would make operations in xpcc very inefficient.
	 */
	template <typename T, typename Allocator = std::allocator<T>>
	class LinkedList : public DoublyLinkedList<T, Allocator>
	{
	public:
		using DoublyLinkedList<T, Allocator>::DoublyLinkedList;
		using iterator = DoublyLinkedList<T, Allocator>::iterator;

		iterator
		remove(const iterator& position)
		{
			return this->erase(position);
		}

		void
		removeAll()
		{
			while (!this->isEmpty()) {
				this->removeFront();
			}
		}
	};
}
#endif	// MODM_LINKED_LIST_HPP

/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Thorsten Lajewski
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_DOUBLY_LINKED_LIST_HPP
#define	MODM_DOUBLY_LINKED_LIST_HPP

#include <list>

namespace modm
{
	/**
	 * \brief	doubly-linked list
	 *
	 * \tparam	T	type of list entries
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_container
	 */
	template <typename T, typename Allocator = std::allocator<T>>
	class [[deprecated("Use std::list instead")]] DoublyLinkedList
	{
	public:
		using const_iterator = std::list<T>::const_iterator;
		using iterator = std::list<T>::iterator;
		using Size = std::size_t;

		DoublyLinkedList(const Allocator& allocator = Allocator())
			: data_{std::move(allocator)}
		{}

		/// check if there are any nodes in the list
		bool
		isEmpty() const
		{
			return data_.empty();
		}

		/// Get number of items in the list
		std::size_t
		getSize() const
		{
			return data_.size();
		}

		/// Insert in front
		bool
		prepend(const T& value)
		{
			data_.push_front(value);
			return true;
		}

		/// Insert at the end of the list
		bool
		append(const T& value)
		{
			data_.push_back(value);
			return true;
		}

		/// Remove the first entry
		void
		removeFront()
		{
			data_.pop_front();
		}

		void
		removeBack()
		{
			data_.pop_back();
		}

		T&
		getFront()
		{
			return data_.front();
		}

		/**
		 * \return the first node in the list
		 */
		const T&
		getFront() const
		{
			return data_.front();
		}

		T&
		getBack()
		{
			return data_.back();
		}

		/**
		 * \return the last node in the list
		 */
		const T&
		getBack() const
		{
			return data_.back();
		}

		/**
		 * Returns a read/write iterator that points to the first element in the
		 * list.  Iteration is done in ordinary element order.
		 */
		iterator
		begin()
		{
			return data_.begin();
		}

		/**
		 * Returns a read-only (constant) iterator that points to the
		 * first element in the list.  Iteration is done in ordinary
		 * element order.
		 */
		const_iterator
		begin() const
		{
			return data_.begin();
		}

		/**
		 * Returns a read/write iterator that points one past the last
		 * element in the list. Iteration is done in ordinary element
		 * order.
		 */
		iterator
		end()
		{
			return data_.end();
		}

		/**
		 * Returns a read-only (constant) iterator that points one past
		 * the last element in the list.  Iteration is done in ordinary
		 * element order.
		 */
		const_iterator
		end() const
		{
			return data_.end();
		}

		/**
		 * Deletes element pointed to by iterator and returns an iterator
		 * to the next element behind the deleted one.
		 *
		 * Warning: you must not use the iterator after calling erase()
		 */
		iterator
		erase(iterator position)
		{
			if (position != data_.end()) {
				return data_.erase(position);
			} else {
				return data_.end();
			}
		}

		/**
		 * Insert data after position iterator.
		 *
		 * This behavior is compatible with modm::LinkedList but different
		 * compared to std::list which inserts before the position iterator
		 * argument.
		 */
		bool
		insert(iterator position, const T& value)
		{
			if (position == data_.end()) {
				data_.push_back(value);
			} else {
				data_.insert(std::next(position), value);
			}
			return true;
		}

	private:
		std::list<T, Allocator> data_;
	};
}

#endif	// MODM_DOUBLY_LINKED_LIST_HPP

/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Thorsten Lajewski
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
#define	MODM_DOUBLY_LINKED_LIST_HPP

#include <stdint.h>
#include <modm/utils/allocator.hpp>
#include <iterator>

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
	template <typename T, typename Allocator = allocator::Dynamic<T> >
	class DoublyLinkedList
	{
	public:
		DoublyLinkedList(const Allocator& allocator = Allocator());

		~DoublyLinkedList();

		/// check if there are any nodes in the list
		inline bool
		isEmpty() const;

		/**
		 * \brief	Get number of items in the list
		 *
		 * Very slow for a long list as it needs to iterate through all
		 * items in the list.
		 */
		std::size_t
		getSize() const;

		/// Insert in front
		bool
		prepend(const T& value);

		/// Insert at the end of the list
		void
		append(const T& value);

		/// Remove the first entry
		void
		removeFront();

		void
		removeBack();

		/**
		 * \return the first node in the list
		 */
		inline const T&
		getFront() const;

		/**
		 * \return the last node in the list
		 */
		inline const T&
		getBack() const;

	protected:
		struct Node
		{
			T value;

			Node *previous;
			Node *next;
		};

		// The stored instance is not actually of type Allocator. Instead we
		// rebind the type to Allocator<Node<T>>. Node<T> is not the same
		// size as T (it's one pointer larger), and specializations on T may go
		// unused because Node<T> is being bound instead.
		typedef typename Allocator::template rebind< Node >::other NodeAllocator;

		NodeAllocator nodeAllocator;

		Node *front;
		Node *back;

	public:
		/**
		 * \brief	Forward iterator
		 *
		 * \todo	decrement operator doesn't work correctly
		 */
		class iterator : public std::iterator<std::forward_iterator_tag, T>
		{
			friend class DoublyLinkedList;
			friend class const_iterator;

		public:
			/// Default constructor
			iterator();
			iterator(const iterator& other);

			iterator& operator = (const iterator& other);
			iterator& operator ++ ();
			iterator& operator -- ();
			bool operator == (const iterator& other) const;
			bool operator != (const iterator& other) const;
			T& operator * ();
			T* operator -> ();

		private:
			iterator(Node* node);

			Node* node;
		};

		/**
		 * \brief	forward const iterator
		 *
		 * \todo	decrement operator doesn't work correctly
		 */
		class const_iterator : public std::iterator<std::forward_iterator_tag, T>
		{
			friend class DoublyLinkedList;

		public:
			/// Default constructor
			const_iterator();

			/**
			 * \brief	Copy constructor
			 *
			 * Used to convert a normal iterator to a const iterator.
			 * The other way is not possible.
			 */
			const_iterator(const iterator& other);

			/**
			 * \brief	Copy constructor
			 */
			const_iterator(const const_iterator& other);

			const_iterator& operator = (const const_iterator& other);
			const_iterator& operator ++ ();
			const_iterator& operator -- ();
			bool operator == (const const_iterator& other) const;
			bool operator != (const const_iterator& other) const;
			const T& operator * () const;
			const T* operator -> () const;

		private:
			const_iterator(const Node* node);

			const Node* node;
		};

		/**
		 * Returns a read/write iterator that points to the first element in      the
		 * list.  Iteration is done in ordinary element order.
		 */
		iterator
		begin();

		/**
		 * Returns a read-only (constant) iterator that points to the
		 * first element in the list.  Iteration is done in ordinary
		 * element order.
		 */
		const_iterator
		begin() const;

		/**
		 * Returns a read/write iterator that points one past the last
		 * element in the list. Iteration is done in ordinary element
		 * order.
		 */
		iterator
		end();

		/**
		 * Returns a read-only (constant) iterator that points one past
		 * the last element in the list.  Iteration is done in ordinary
		 * element order.
		 */
		const_iterator
		end() const;

		/**
		 * Deletes element pointed to by iterator and returns an iterator
		 * to the next element behind the deleted one.
		 *
		 * Warning: you must not use the iterator after calling erase()
		 */
		iterator
		erase(iterator position);

	private:
		friend class const_iterator;
		friend class iterator;

		DoublyLinkedList(const DoublyLinkedList& other);

		DoublyLinkedList&
		operator = (const DoublyLinkedList& other);
	};
}

#include "doubly_linked_list_impl.hpp"
#include "doubly_linked_list_iterator_impl.hpp"

#endif	// MODM_DOUBLY_LINKED_LIST_HPP

/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Thorsten Lajewski
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
#define	MODM_LINKED_LIST_HPP

#include <stdint.h>
#include <iterator>
#include <modm/utils/allocator.hpp>

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
	 */
	template <typename T, typename Allocator = allocator::Dynamic<T> >
	class LinkedList
	{
	public:
		typedef std::size_t Size;

	public:
		LinkedList(const Allocator& allocator = Allocator());

		~LinkedList();

		/// check if there are any nodes in the list
		inline bool
		isEmpty() const;

		/**
		 * \brief	Get number of elements
		 *
		 * \warning	This method is slow because it has to iterate through
		 * 			all elements.
		 */
		std::size_t
		getSize() const;

		/// Insert in front
		bool
		prepend(const T& value);

		/// Insert at the end of the list
		bool
		append(const T& value);

		/// Remove the first entry
		void
		removeFront();

		/**
		 * \return the first node in the list
		 */
		inline const T&
		getFront() const;

		inline T&
		getFront();

		/**
		 * \return the last node in the list
		 */
		inline const T&
		getBack() const;

		inline T&
		getBack();

		/**
		 * \brief	Remove all elements form the list
		 */
		void
		removeAll();

	protected:
		struct Node
		{
			T value;
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
		 */
		class iterator : public std::iterator<std::forward_iterator_tag, T>
		{
			friend class LinkedList;
			friend class const_iterator;

		public:
			/// Default constructor
			iterator();
			iterator(const iterator& other);

			iterator& operator = (const iterator& other);
			iterator& operator ++ ();
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
		 */
		class const_iterator : public std::iterator<std::forward_iterator_tag, T>
		{
			friend class LinkedList;

		public:
			/// Default constructor
			const_iterator();

			/**
			 * \brief	Copy construtor
			 *
			 * Used to convert a normal iterator to a const iterator.
			 * The other way is not possible.
			 */
			const_iterator(const iterator& other);

			/**
			 * \brief	Copy construtor
			 */
			const_iterator(const const_iterator& other);

			const_iterator& operator = (const const_iterator& other);
			const_iterator& operator ++ ();
			bool operator == (const const_iterator& other) const;
			bool operator != (const const_iterator& other) const;
			const T& operator * () const;
			const T* operator -> () const;

		private:
			const_iterator(Node* node);
			// TODO: this should acutally be a node that points to a const
			// value, but since all access is const, this does not really make
			// a difference
			Node* node;
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
		 * \brief	Erase element
		 *
		 * Removes a single element from the list container.
		 * This effectively reduces the list size by one, calling the element's
		 * destructor before.
		 */
		iterator
		remove(const iterator& position);

		bool
		insert(const_iterator pos, const T& value);

	private:
		friend class const_iterator;
		friend class iterator;

		LinkedList(const LinkedList& other);

		LinkedList&
		operator = (const LinkedList& other);
	};
}

#include "linked_list_impl.hpp"
#include "linked_list_iterator_impl.hpp"

#endif	// MODM_LINKED_LIST_HPP

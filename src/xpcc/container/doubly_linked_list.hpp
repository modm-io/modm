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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__DOUBLY_LINKED_LIST_HPP
#define	XPCC__DOUBLY_LINKED_LIST_HPP

#include <stdint.h>
#include <xpcc/utils/allocator.hpp>

namespace xpcc
{
	/**
	 * \brief	doubly-linked list
	 * 
	 * \tparam	T	type of list entries
	 * 
	 * \author	Fabian Greif
	 * \ingroup	container
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
		
		/// Insert in front
		void
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
		class iterator
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
		class const_iterator
		{
			friend class DoublyLinkedList;
			
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

#endif	// XPCC__DOUBLY_LINKED_LIST_HPP

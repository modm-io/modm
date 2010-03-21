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

#ifndef	XPCC__LIST_HPP
#define	XPCC__LIST_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \brief	Single-linked list
	 * 
	 * \tparam	T	type of list entries
	 * 
	 * \warning	The node passed to the functions must allways be valid
	 * 			List::Node object 	 
	 * \todo	This implementation needs an iterator interface!
	 *
	 * \author	Fabian Greif
	 * \ingroup	data_structure
	 */
	template <typename T>
	class List
	{
	public:
		/**
		 * \brief	One item of the list
		 */
		struct Node
		{
			Node(const T& initalValue = T()) :
				next(0), value(initalValue)
			{
			}
			
			void
			setValue(const T& newValue)
			{
				this->value = newValue;
			}
			
			inline const T&
			getValue() const
			{
				return value;
			}
			
			inline T&
			getValue()
			{
				return value;
			}
			
			inline Node*
			operator ++ () const
			{
				return this->next;
			}

		private:
			Node *next;
			T value;
		
		private:
			friend class List;
		};
		
		/**
		 * \brief	Efficient way of creating a pool of nodes
		 * 
		 * Creates an array of nodes.
		 * 
		 * \code
		 * typedef xpcc::List<uint16_t> MyList;
		 * 
		 * MyList::NodeFactory<3> *factory = new MyList::NodeFactory<3>();
		 * MyList list;
		 * 
		 * list.append(factory->getNode(1));
		 * list.append(factory->getNode(-1123));
		 * \endcode
		 * 
		 * \todo perhaps this class should become an allocator for the list class?
		 */
		template<int N>
		class NodeFactory
		{
		public:
			NodeFactory();
			
			Node *
			getNode();
			
			Node *
			getNode(const T& value);
			
			void
			freeNode(Node *node);
			
			bool
			isEmpty() const;
			
		private:
			Node pool[N];
			Node *first;
		
		private:
			friend class Node;
		};
		
	public:
		List();
		
		
		/// Insert in front
		void
		prepend(Node *node);

		/// Insert at the end of the list
		void
		append(Node *node);

		/// Insert node after a given node
		void
		insertAfter(Node *previous, Node *newNode);
		
		/**
		 * \brief	Remove an node form the list
		 * 
		 * This method has to iterate through the list to find the predecessor
		 * of the node to be removed.
		 * 
		 * Therefore removeAfter() and removeFirst() should be prefered.
		 */
		Node*
		remove(Node *node);
		
		/**
		 * \brief	Remove the node after the given node
		 */
		Node*
		removeAfter(Node *previous);
		
		/**
		 * Remove the first entry
		 * 
		 * \warning	The method doesn't check if there is a first entry. This
		 * 			have to be done by a call to isEmpty() before.
		 */
		Node*
		removeFirst();
		
		/// check if there are any nodes in the list
		inline bool
		isEmpty() const;
		
		
		/**
		 * \brief	Access the node at position \a index
		 * 
		 * \warning	The implementation has to iterate through the list
		 * 			until it reaches the desired position. Therefore an
		 * 			access via iterator is prefered.
		 */
		Node*
		at(int index) const;

		/**
		 * \return the first node in the list
		 */
		inline Node*
		front() const;

		/**
		 * \return the last node in the list
		 */
		inline Node*
		back() const;
	
	private:
		Node *first;
		Node *last;
	};
}

#include "list_impl.hpp"
#include "list_factory_impl.hpp"

#endif	// XPCC__LIST_HPP

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

namespace xpcc
{
	/**
	 * \brief	Single-linked list
	 * 
	 * \tparam	T	type of list entries
	 * 
	 * \warning	The node passed to the functions must allways be valid
	 * 			List::Node object
	 * \ingroup	data_structure
	 * 
	 * \todo	This implementation needs an iterator interface!
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
			friend class List;
			
			Node(const T& value = T()) :
				next(0), value(value)
			{
			}
			
			T&
			getValue()
			{
				return value;
			}
			
		private:
			Node *next;
			T value;
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
		insertAfter(Node *current, Node *newNode);
		
		/// remove an node form the list
		bool
		remove(Node *node);
		
		
		/// check if there are any nodes in the list
		bool
		isEmpty() const;
		
		
		/**
		 * \brief	Access the node at position \a index
		 * 
		 * \warning	The implementation has to iterate throu the list
		 * 			until it reaches the desired position. Therefore an
		 * 			access via iterator is prefered.
		 */
		Node*
		at(int index) const;
	
	private:
		Node *first;
		Node *last;
	};
}

#include "list_impl.hpp"

#endif	// XPCC__LIST_HPP

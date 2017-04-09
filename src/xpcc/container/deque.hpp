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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__DEQUE_HPP
#define	XPCC__DEQUE_HPP

#include <cstddef>

#include <stdint.h>
#include <xpcc/utils/template_metaprogramming.hpp>

namespace xpcc
{
	/**
	 * \brief	Double ended queue
	 * 
	 * Internally organized as a ring buffer.
	 * 
	 * \verbatim
	 *           tail --\                 /-- head
	 *                  |                 |
	 *        +------+------+---- ----+------+------+
	 *      0 |      | data |   ...   | data |      | N-1
	 *        +------+------+---- ----+------+------+
	 *           |      |                 |      |
	 * prepend --/      |                 |      \-- append
	 *                  |                 |
	 *       getFront --/                 \-- getBack
	 * \endverbatim
	 * 
	 * \tparam	T	Type of the elements
	 * \tparam	N	Size of the queue
	 * 
	 * Up to a size of 254 small index variables with 8-bits are used, after
	 * this they are switched to 16-bit.
	 * 
	 * \warning	This class don't check if the container is empty before
	 * 			a pop-operation. You have to do this by yourself!
	 * 
	 * \author		Fabian Greif
	 * \ingroup		container
	 */
	template<typename T,
			 std::size_t N>
	class BoundedDeque
	{
	public:
		// select the type of the index variables with some template magic :-)
		typedef typename xpcc::tmp::Select< (N >= 255),
											uint_fast16_t,
											uint_fast8_t >::Result Index;
		
		typedef Index Size;
		
	public:
		BoundedDeque();
		
		inline bool
		isEmpty() const;

		inline bool
		isNotEmpty() const { return not isEmpty(); };
		
		inline bool
		isFull() const;

		inline bool
		isNotFull() const { return not isFull(); };
		
		inline Size
		getSize() const;
		
		inline Size
		getMaxSize() const;
		
		/**
		 * \brief	Clear the container
		 *
		 * \warning	This will discard all the items in the container
		 */
		void
		clear();
		
		
		inline T&
		getFront();
		
		inline const T&
		getFront() const;
		
		/**
		 * \brief	Get item at specified index
		 * 
		 * Returns a reference to the item at index `n`, counting 0-indexed
		 * from Front to Back, which is the same order in that the items were
		 * appended and the iterator addresses them.
		 * 
		 * \warning Please make sure `n` is a valid index: 0 <= *n* < *size*.
		 * 			Other indexes will cause undefined behaviour.
		 */
		inline T&
		get(Index n);
		
		inline const T&
		get(Index n) const;
		
		/**
		 * \brief	Get item at specified index
		 * 
		 * Returns a reference to the item at index `n`, counting 0-indexed
		 * from Front to Back.
		 * 
		 * \warning Please make sure `n` is a valid index: 0 <= *n* < *size*.
		 * 			Other indexes will cause undefined behaviour.
		 */
		inline T&
		operator[](Index n);
		
		inline const T&
		operator[](Index n) const;
		
		/**
		 * \brief	Get item at specified index
		 * 
		 * Returns a reference to the item at index `n`. The items are indexed
		 * in reverse (Back to Front), which is their order when they have been
		 * prepended.
		 * 
		 * \warning Please make sure `n` is a valid index: 0 <= *n* < *size*.
		 * 			Other indexes will cause undefined behaviour.
		 */
		inline T&
		rget(Index n);
		
		inline const T&
		rget(Index n) const;
		
		inline T&
		getBack();
		
		inline const T&
		getBack() const;
		
		bool
		append(const T& value);
		
		/**
		 * \brief	Append an item to the back of the deque overwriting existing items
		 * 
		 * This method, in contrast to `append()`, overwrites existing items in the deque
		 * if it is full. When an item is appended to the already full deque, the front
		 * item is removed to use its space for the new item, which is inserted after back.
		 */
		void
		appendOverwrite(const T& value);
		
		bool
		prepend(const T& value);
		
		/**
		 * \brief	Prepend an item to the front of the deque overwriting existing items
		 * 
		 * This method, in contrast to `prepend()`, overwrites existing items in the deque
		 * if it is full. When an item is prepended to the already full deque, the back
		 * item is removed to use its space for the new item, which is inserted at front.
		 */
		void
		prependOverwrite(const T& value);
		
		void
		removeBack();
		
		void
		removeFront();
	
	public:
		/**
		 * \brief	Bidirectional const iterator
		 *
		 * \todo	check if a simpler implementation is possible
		 */
		class const_iterator
		{
			friend class BoundedDeque;
			
		public:
			const_iterator();
			const_iterator(const const_iterator& other);
			
			const_iterator& operator = (const const_iterator& other);
			const_iterator& operator ++ ();
			const_iterator& operator -- ();
			bool operator == (const const_iterator& other) const;
			bool operator != (const const_iterator& other) const;
			const T& operator * () const;
			const T* operator -> () const;
		
		private:
			const_iterator(Index index, const BoundedDeque * parent);
			
			Index index;
			const BoundedDeque * parent;
			
			Size count;
		};
		
		const_iterator
		begin() const;
		
		const_iterator
		end() const;
		
	private:
		friend class const_iterator;
		
		Index head;
		Index tail;
		Size size;
		
		T buffer[N];
	};
}

#include "deque_impl.hpp"

#endif	// XPCC__DEQUE_HPP

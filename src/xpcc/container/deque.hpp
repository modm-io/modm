/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__DEQUE_HPP
#define	XPCC__DEQUE_HPP

#include <cstddef>

#include <stdint.h>
#include <modm/utils/template_metaprogramming.hpp>

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
		
		inline T&
		getBack();
		
		inline const T&
		getBack() const;
		
		
		bool
		append(const T& value);
		
		bool
		prepend(const T& value);
		
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

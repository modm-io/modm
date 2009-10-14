// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#ifndef	XPCC__DEQUE_HPP
#define	XPCC__DEQUE_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \ingroup	data_structure
	 * \brief	Double ended queue
	 * 
	 * Internally organised as a ring buffer.
	 * 
	 * \verbatim
	 *              tail --\                        /-- head
	 *                     |                        |
	 *           +------+------+---- ----+------+------+
	 *           |      | data |   ...   | data |      |
	 *           +------+------+---- ----+------+------+
	 *              |      |                 |      |
	 * push_front --/      |                 |      \-- push_back
	 *                     |                 |
	 *             front --/                 \-- back
	 * \endverbatim
	 * 
	 * \tparam	T	Type of the elements
	 * \tparam	N	Size of the queue
	 * \tparam	S	Type of status Variables (use uint8_t for N < 255 (default),
	 * 				otherwise uint16_t)
	 * 
	 * \warning		This class don't check if the container is not empty before
	 * 				a pop operation. You have to do this by yourself!
	 */
	template<typename T,
			 int N,
			 typename S=uint8_t>
	class BoundedDeque
	{
	public:
		typedef S SizeType;
		
		BoundedDeque() : head(0), tail(0) {}
		
		bool
		isEmpty() const;
		
		bool
		isFull() const;
		
		// TODO
		//size_type
		//getSize() const;
		
		SizeType
		getMaxSize() const;
		
		// TODO
		//void
		//clear();
		
		
		T&
		front();
		
		const T&
		front() const;
		
		T&
		back();
		
		const T&
		back() const;
		
		
		bool
		pushBack(const T& value);
		
		bool
		pushFront(const T& value);
		
		void
		popBack();
		
		void
		popFront();
	
	private:
		S head;
		S tail;
		
		T buffer[N];
	};
}

#include "deque_impl.hpp"

#endif	// XPCC__DEQUE_HPP

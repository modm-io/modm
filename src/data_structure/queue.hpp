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

#ifndef	XPCC_QUEUE_HPP
#define	XPCC_QUEUE_HPP

#include "deque.hpp"

namespace xpcc
{
	// ------------------------------------------------------------------------
	/**
	 * \ingroup	container
	 * \brief	FIFO queue
	 * 
	 * Elements are pushed into the "back" of the specific container and popped 
	 * from its "front".
	 * 
	 * This class is thread-safe for S = uint8_t if one thread only writes data 
	 * (using push()) and the other only reads data (using front() and pop()).
	 * 
	 * \see		Deque()
	 */
	template<typename T,
			 typename Container>
	class Queue
	{
	public:
		typedef typename Container::SizeType SizeType;
		
		bool
		isEmpty() {
			return c.isEmpty();
		}
		
		bool
		isFull() {
			return c.isFull();
		}
		
		SizeType
		getSize() {
			return c.getSize();
		}
		
		SizeType
		getMaxSize() {
			return c.getMaxSize();
		}
		
		T&
		get() {
			return c.front();
		}
		
		const T&
		get() const {
			return c.front();
		}
		
		bool
		push(const T& value) {
			return c.pushBack(value);
		}
		
		void
		pop() {
			c.popFront();
		}

	protected:
		Container c;
	};


	// ------------------------------------------------------------------------
	template<typename T, 
			 int N,
			 typename S=uint8_t,
			 typename Container=BoundedDeque<T, N, S> >
	class BoundedQueue : public Queue<T, Container> {
		
	};

}

#endif	// XPCC_QUEUE_HPP

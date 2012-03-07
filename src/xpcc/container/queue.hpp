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
 * $Id: queue.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__QUEUE_HPP
#define	XPCC__QUEUE_HPP

#include <cstddef>

#include "deque.hpp"

namespace xpcc
{
	/**
	 * \brief	FIFO queue
	 * 
	 * getFront(), removeFront(), append()
	 * 
	 * \tparam	T			type
	 * \tparam	Container	container
	 * 
	 * \see		Deque()
	 * 
	 * \author	Fabian Greif
	 * \ingroup	container
	 */
	template<typename T,
			 typename Container>
	class Queue
	{
	public:
		typedef typename Container::Size Size;
		
	public:
		inline bool
		isEmpty()
		{
			return c.isEmpty();
		}
		
		inline bool
		isFull()
		{
			return c.isFull();
		}
		
		inline Size
		getSize()
		{
			return c.getSize();
		}
		
		inline Size
		getMaxSize()
		{
			return c.getMaxSize();
		}
		
		/// Access first element
		inline T&
		get()
		{
			return c.getFront();
		}
		
		/// Access first element
		inline const T&
		get() const
		{
			return c.getFront();
		}
		
		inline bool
		push(const T& value)
		{
			return c.append(value);
		}
		
		inline void
		pop()
		{
			c.removeFront();
		}

	protected:
		Container c;
	};


	// ------------------------------------------------------------------------
	/**
	 * \brief	Bounded queue
	 * \ingroup	container
	 */
	template<typename T, 
			 std::size_t N,
			 typename Container = BoundedDeque<T, N> >
	class BoundedQueue : public Queue<T, Container>
	{
	};

}

#endif	// XPCC__QUEUE_HPP

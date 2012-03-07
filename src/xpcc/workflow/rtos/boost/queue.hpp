// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
 * $Id: queue.hpp 700 2012-01-19 12:53:58Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BOOST__QUEUE_HPP
#define XPCC_BOOST__QUEUE_HPP

#include <stdint.h>
#include <deque>

#include <boost/thread/mutex.hpp>
#include <xpcc/container/deque.hpp>

namespace xpcc
{
	namespace rtos
	{
		/**
		 * Thread-safe Queue.
		 * 
		 * \ingroup	rtos_boost
		 */
		template<typename T>
		class Queue
		{
		public:
			/**
			 * Create a Queue.
			 * 
			 * \param length
			 * 			The maximum number of items the queue can contain.
			 */
			Queue(uint32_t length);
			
			~Queue();
			
			/**
			 * Get the number of items stored in the queue
			 */
			std::size_t
			getSize() const;
			
			bool
			append(const T& item, uint32_t timeout = -1);
			
			bool
			prepend(const T& item, uint32_t timeout = -1);
			
			
			bool
			peek(T& item, uint32_t timeout = -1) const;
			
			bool
			get(T& item, uint32_t timeout = -1);
			
			
			inline bool
			appendFromInterrupt(const T& item);
			
			inline bool
			prependFromInterrupt(const T& item);
			
			inline bool
			getFromInterrupt(T& item);
			
		private:
			// disable copy constructor
			Queue(const Queue& other);
			
			// disable assignment operator
			Queue&
			operator = (const Queue& other);
			
			mutable boost::timed_mutex mutex;
			
			uint32_t maxSize;
			std::deque<T> deque;
		};
	}
}

#endif // XPCC_BOOST__QUEUE_HPP

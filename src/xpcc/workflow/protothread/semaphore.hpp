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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PT__SEMAPHORE_HPP
#define XPCC_PT__SEMAPHORE_HPP

#include <stdint.h>
#include "macros.hpp"

namespace xpcc
{
	namespace pt
	{
		/**
		 * \brief	Counting semaphore
		 * 
		 * This module implements counting semaphores on top of
		 * protothreads. Semaphores are a synchronization primitive that
		 * provide two operations: acquire() and release(). Sometimes these
		 * operations are also called \c wait and \c signal. The acquire()
		 * operation checks the semaphore counter and "blocks" the thread if
		 * the counter is zero.
		 * The release() operation increases the semaphore counter but
		 * does not block. If another thread has blocked waiting for the
		 * semaphore that is signalled, the blocked thread will become
		 * runnable again.
		 * 
		 * This class can for example be used for resouce limiting or credit
		 * tracking:
		 * 
		 * Resouce limit:
		 * - \p initial = number of resources
		 * - acquire() to gain access to a resource
		 * - release() to release the resource
		 * 
		 * Credit tracking:
		 * - \p initial = 0
		 * - acquire() to increment (credit)
		 * - release() to consume (debit)
		 * 
		 * As protothreads implement a form cooperative multithreading no
		 * synchronisation is needed for accessing global variables or for
		 * critical sections. Just don't use PT_YIELD() or PT_WAIT_UNTIL()
		 * inside a critical sections and you're fine.
		 * 
		 * Therefore there's no Mutex implementation, it isn't needed.
		 * 
		 * \ingroup	protothread
		 */
		class Semaphore
		{
		public:
			/**
			 * @brief	Create a counting semaphore
			 * 
			 * @param	initial		Initial value
			 */
			Semaphore(uint16_t initial) : 
				count(initial)
			{
			}
			
			/**
			 * \brief	Acquire the semaphore
			 * 
			 * The "acquire" or "wait" operation causes the protothread to
			 * block while the counter is zero. When the counter reaches a 
			 * value larger than zero, the protothread will continue.
			 * 
			 * Use this always within a PT_WAIT_UNITL() macro inside
			 * a xpcc::pt::Thread::run() method.
			 * 
			 * \code
			 * PT_WAIT_UNTIL(semaphore.acquire());
			 * \endcode
			 * 
			 * \return	\c false if the Semaphore count not be acquired,
			 * 			\c true if the operation was successful.
			 */
			bool
			acquire()
			{
				if (this->count > 0)
				{
					this->count--;
					return true;
				}
				else {
					return false;
				}
			}
			
			/**
			 * @brief	Release the semaphore
			 * 
			 * The "release" or "signal" operation increments the counter
			 * inside the semaphore, which eventually will cause waiting
			 * protothreads to continue executing.
			 */
			void
			release()
			{
				this->count++;
			}
			
		protected:
			uint16_t count;
		};
	}
}

#endif // XPCC_PT__SEMAPHORE_HPP

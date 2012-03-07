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
 * $Id: mutex.hpp 700 2012-01-19 12:53:58Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BOOST__MUTEX_HPP
#define XPCC_BOOST__MUTEX_HPP

#include <boost/thread/mutex.hpp>

namespace xpcc
{
	namespace rtos
	{
		// forward declaration
		class MutexGuard;
		
		/**
		 * \brief	Mutex
		 * 
		 * \ingroup	boost_rtos
		 */
		class Mutex
		{
			friend class MutexGuard;
			
		public:
			Mutex();
			
			~Mutex();
			
			/**
			 * \param	timeout		Timeout in Milliseconds
			 */
			bool
			acquire(uint32_t timeout);
			
			inline void
			acquire()
			{
				mutex.lock();
			}
			
			inline void
			release()
			{
				mutex.unlock();
			}
			
		private:
			// disable copy constructor
			Mutex(const Mutex& other);
			
			// disable assignment operator
			Mutex&
			operator = (const Mutex& other);
			
			boost::timed_mutex mutex;
		};
		
		/**
		 * Implements a RAII-style locking.
		 * 
		 * Locks the Mutex when created and unlocks it on destruction.
		 */
		class MutexGuard : boost::lock_guard<boost::timed_mutex>
		{
		public:
			MutexGuard(Mutex& m) :
				boost::lock_guard<boost::timed_mutex>(m.mutex)
			{
			}
			
			~MutexGuard()
			{
			}
		};
	}
}

#endif // XPCC_BOOST__MUTEX_HPP

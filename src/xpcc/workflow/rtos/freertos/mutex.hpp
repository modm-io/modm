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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FREERTOS__MUTEX_HPP
#define XPCC_FREERTOS__MUTEX_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace xpcc
{
	namespace rtos
	{
		/**
		 * \brief	Mutex
		 * 
		 * Mutexes and binary semaphores are very similar but have some subtle
		 * differences: Mutexes include a priority inheritance mechanism,
		 * binary semaphores do not.
		 * 
		 * This makes binary semaphores the better choice for implementing
		 * synchronisation (between tasks or between tasks and an interrupt),
		 * and mutexes the better choice for implementing simple mutual exclusion.
		 * 
		 * \ingroup	freertos
		 */
		class Mutex
		{
		public:
			Mutex();
			
			~Mutex();
			
			bool
			acquire(portTickType timeout = portMAX_DELAY);
			
			void
			release();
			
		private:
			// disable copy constructor
			Mutex(const Mutex& other);
			
			// disable assignment operator
			Mutex&
			operator = (const Mutex& other);
			
			xSemaphoreHandle handle;
		};
		
		/**
		 * Implements a RAII-style locking.
		 * 
		 * Locks the Mutex when created and unlocks it on destruction.
		 */
		class MutexGuard
		{
		public:
			MutexGuard(Mutex& m) :
				mutex(m)
			{
				mutex.acquire();
			}
			
			~MutexGuard()
			{
				mutex.release();
			}
			
		private:
			Mutex& mutex;
		};
	}
}

#endif // XPCC_FREERTOS__MUTEX_HPP

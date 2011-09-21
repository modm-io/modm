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

#ifndef XPCC_FREERTOS__SEMAPHORE_HPP
#define XPCC_FREERTOS__SEMAPHORE_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace xpcc
{
	namespace freertos
	{
		/**
		 * \brief	Base class for binary and counting semaphores
		 * \internal
		 */
		class SemaphoreBase
		{
		public:
			~SemaphoreBase();
			
			/**
			 * \brief	Aquire the semaphore
			 * 
			 * Decrements the internal count. This function might be called
			 * 'take' or 'wait' in other implementations.
			 */
			bool
			acquire(portTickType timeout = portMAX_DELAY);
			
			/**
			 * \brief	Release the semaphore
			 * 
			 * Increments the internal count. This function might be called
			 * 'give' or 'signal' in other implementations.
			 */
			void
			release();
			
			/**
			 * \brief	Release the Semaphore from within an interrupt context
			 * 
			 * Might request an context switch after finishing the interrupt
			 * if a higher priority task has been woken through the release.
			 */
			void
			releaseFromInterrupt();
			
		protected:
			SemaphoreBase()
			{
			}
			
			xSemaphoreHandle handle;
			
		private:
			// disable copy constructor
			SemaphoreBase(const SemaphoreBase&);
			
			// disable assignment operator
			SemaphoreBase &
			operator = (const SemaphoreBase&);
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	Counting semaphore
		 * 
		 * Counting semaphores are typically used for two things:
		 * 
		 * -# Counting events. \n
		 *    In this usage scenario an event handler will 'release' a semaphore
		 *    each time an event occurs (incrementing the semaphore count
		 *    value), and a handler task will 'acquire' a semaphore each time it
		 *    processes an event (decrementing the semaphore count value). The
		 *    count value is therefore the difference between the number of
		 *    events that have occurred and the number that have been
		 *    processed. In this case it is desirable for the initial count
		 *    value to be zero.
		 * 
		 * -# Resource management.
		 *    In this usage scenario the count value indicates the number of
		 *    resources available. To obtain control of a resource a task must
		 *    first obtain a semaphore - decrementing the semaphore count
		 *    value. When the count value reaches zero there are no free
		 *    resources. When a task finishes with the resource it 'gives' the
		 *    semaphore back (release()) - incrementing the semaphore count value.
		 *    In this case it is desirable for the initial count value to be
		 *    equal to the maximum count value, indicating that all resources
		 *    are free.
		 * 
		 * \ingroup	freertos
		 */
		class Semaphore : public SemaphoreBase
		{
		public:
			/**
			 * \brief	Create a counting semaphore
			 * 
			 * \param	max		Maximum count value
			 * \param	initial	Initial value
			 */
			Semaphore(unsigned portBASE_TYPE max,
					unsigned portBASE_TYPE initial);
			
		private:
			// disable copy constructor
			Semaphore(const Semaphore&);
			
			// disable assignment operator
			Semaphore &
			operator = (const Semaphore&);
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	Binary semaphore
		 * 
		 * The semaphore is released by default.
		 * 
		 * \ingroup	rtos
		 */
		class BinarySemaphore : public SemaphoreBase
		{
		public:
			BinarySemaphore();
			
		private:
			// disable copy constructor
			BinarySemaphore(const BinarySemaphore&);
			
			// disable assignment operator
			BinarySemaphore &
			operator = (const BinarySemaphore&);
		};
	}
}

#endif // XPCC_FREERTOS__SEMAPHORE_HPP

/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FREERTOS_SEMAPHORE_HPP
#define XPCC_FREERTOS_SEMAPHORE_HPP

#ifndef XPCC_RTOS_SEMAPHORE_HPP
#	error "Don't include this file directly, use <modm/processing/rtos/semaphore.hpp>"
#endif

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace xpcc
{
	namespace rtos
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
			 * if a higher priority thread has been woken through the release.
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
		 *    value), and a handler thread will 'acquire' a semaphore each time it
		 *    processes an event (decrementing the semaphore count value). The
		 *    count value is therefore the difference between the number of
		 *    events that have occurred and the number that have been
		 *    processed. In this case it is desirable for the initial count
		 *    value to be zero.
		 * 
		 * -# Resource management.
		 *    In this usage scenario the count value indicates the number of
		 *    resources available. To obtain control of a resource a thread must
		 *    first obtain a semaphore - decrementing the semaphore count
		 *    value. When the count value reaches zero there are no free
		 *    resources. When a thread finishes with the resource it 'gives' the
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

#endif // XPCC_FREERTOS_SEMAPHORE_HPP

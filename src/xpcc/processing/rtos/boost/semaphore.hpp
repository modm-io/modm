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

#ifndef XPCC_BOOST_SEMAPHORE_HPP
#define XPCC_BOOST_SEMAPHORE_HPP

#ifndef XPCC_RTOS_SEMAPHORE_HPP
#	error "Don't include this file directly, use <modm/processing/rtos/semaphore.hpp>"
#endif

#include <boost/thread/mutex.hpp>    
#include <boost/thread/condition_variable.hpp>

namespace xpcc
{
	namespace rtos
	{
		/**
		 * Counting semaphore.
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
		 * \ingroup	rtos_boost
		 */
		class Semaphore
		{
		public:
			/**
			 * \brief	Create a counting semaphore
			 * 
			 * \param	max		Maximum count value
			 * \param	initial	Initial value
			 */
			Semaphore(uint32_t max,
					uint32_t initial);
			
			/**
			 * \brief	Aquire the semaphore
			 * 
			 * Decrements the internal count. This function might be called
			 * 'take' or 'wait' in other implementations.
			 */
			bool
			acquire(uint32_t timeout = -1);
			
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
			 */
			inline void
			releaseFromInterrupt()
			{
				release();
			}
			
		private:
			// disable copy constructor
			Semaphore(const Semaphore&);
			
			// disable assignment operator
			Semaphore &
			operator = (const Semaphore&);
			
			// The current semaphore count.
			unsigned int count;
			unsigned int maxCount;
			
			// Mutex protects count.
			// 
			// Any code that reads or writes the count_ data must hold a lock
			// on the mutex.
			mutable boost::mutex mutex;
			
			// Code that increments count_ must notify the condition variable.
			mutable boost::condition_variable condition;

		};
		
		/**
		 * \brief	Binary semaphore
		 * 
		 * The semaphore is released by default.
		 * 
		 * \ingroup	rtos_boost
		 */
		class BinarySemaphore : public Semaphore
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

#endif // XPCC_BOOST_SEMAPHORE_HPP

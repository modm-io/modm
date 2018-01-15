/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FREERTOS_MUTEX_HPP
#define MODM_FREERTOS_MUTEX_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace modm
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
		 * synchronisation (between threads or between threads and an interrupt),
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

#endif // MODM_FREERTOS_MUTEX_HPP

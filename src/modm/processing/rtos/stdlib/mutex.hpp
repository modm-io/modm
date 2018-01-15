/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STDLIB_MUTEX_HPP
#define MODM_STDLIB_MUTEX_HPP

#include <mutex>

namespace modm
{
	namespace rtos
	{
		// forward declaration
		class MutexGuard;
		
		/**
		 * \brief	Mutex
		 * 
		 * \ingroup	stdlib_rtos
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
			
			std::timed_mutex mutex;
		};
		
		/**
		 * Implements a RAII-style locking.
		 * 
		 * Locks the Mutex when created and unlocks it on destruction.
		 */
		class MutexGuard : std::lock_guard<std::timed_mutex>
		{
		public:
			MutexGuard(Mutex& m) :
				std::lock_guard<std::timed_mutex>(m.mutex)
			{
			}
			
			~MutexGuard()
			{
			}
		};
	}
}

#endif // MODM_STDLIB_MUTEX_HPP

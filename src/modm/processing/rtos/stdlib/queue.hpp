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

#ifndef MODM_STDLIB_QUEUE_HPP
#define MODM_STDLIB_QUEUE_HPP

#include <stdint.h>
#include <deque>

#include <mutex>

namespace modm
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
			
			mutable std::timed_mutex mutex;
			
			uint32_t maxSize;
			std::deque<T> deque;
		};
	}
}

#endif // MODM_STDLIB_QUEUE_HPP

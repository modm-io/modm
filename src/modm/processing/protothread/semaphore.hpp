/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PT_SEMAPHORE_HPP
#define MODM_PT_SEMAPHORE_HPP

#include <stdint.h>
#include "macros.hpp"

namespace modm
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
		 * This class can for example be used for resource limiting or credit
		 * tracking:
		 *
		 * Resource limit:
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
		 * \ingroup	modm_processing_protothread
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
			 * a modm::pt::Thread::run() method.
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

#endif // MODM_PT_SEMAPHORE_HPP

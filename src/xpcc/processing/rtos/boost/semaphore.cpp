/*
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

#include "../semaphore.hpp"

// ----------------------------------------------------------------------------
xpcc::rtos::Semaphore::Semaphore(uint32_t max, uint32_t initial) :
	count(initial), maxCount(max)
{
}

// ----------------------------------------------------------------------------
bool
xpcc::rtos::Semaphore::acquire(uint32_t timeout)
{
	boost::unique_lock<boost::mutex> lock(mutex);
	while (count == 0)
	{
		 if (!condition.timed_wait(lock,
				 boost::posix_time::milliseconds(timeout))) {
			 return false;
		 }
	}
	--count;
	
	return true;
}

void
xpcc::rtos::Semaphore::release()
{
	boost::unique_lock<boost::mutex> lock(mutex);
	
	if (count < maxCount) {
		++count;
		
		// Wake up any waiting threads. 
		// Always do this, even if count_ wasn't 0 on entry. Otherwise, we
		// might not wake up enough waiting threads if we get a number of
		// signal() calls in a row.
		condition.notify_one();
	}
}

// ----------------------------------------------------------------------------
xpcc::rtos::BinarySemaphore::BinarySemaphore() :
		Semaphore(1, 1)
{
}

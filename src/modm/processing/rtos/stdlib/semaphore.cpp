/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "semaphore.hpp"
#include <chrono>

// ----------------------------------------------------------------------------
modm::rtos::Semaphore::Semaphore(uint32_t max, uint32_t initial) :
	count(initial), maxCount(max)
{
}

// ----------------------------------------------------------------------------
bool
modm::rtos::Semaphore::acquire(uint32_t timeout)
{
	std::unique_lock<std::mutex> lock(mutex);
	while (count == 0)
	{
		 if (condition.wait_for(lock, std::chrono::milliseconds(timeout)) == std::cv_status::timeout) {
			 return false;
		 }
	}
	--count;

	return true;
}

void
modm::rtos::Semaphore::release()
{
	std::unique_lock<std::mutex> lock(mutex);

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
modm::rtos::BinarySemaphore::BinarySemaphore() :
		Semaphore(1, 1)
{
}

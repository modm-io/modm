/*
 * Copyright (c) 2012, Fabian Greif
 * Copyright (c) 2012, 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>
#include <modm/processing/rtos.hpp>
#include <modm/debug/logger.hpp>

// ----------------------------------------------------------------------------
modm::rtos::BinarySemaphore event;

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

// ----------------------------------------------------------------------------
class Thread1 : public modm::rtos::Thread
{
public:
	Thread1() :
		modm::rtos::Thread(2)
	{
	}

	virtual void
	run()
	{
		while (1)
		{
			MODM_LOG_INFO << "ping" << modm::endl;

			// synchronize with Task2
			event.release();

			this->sleep(1000 * MILLISECONDS);
		}
	}
};

class Thread2 : public modm::rtos::Thread
{
public:
	Thread2() :
		modm::rtos::Thread(2)
	{
	}

	virtual void
	run()
	{
		while (1)
		{
			MODM_LOG_INFO << "pong" << modm::endl;

			// wait for the other task
			event.acquire();
		}
	}
};

// Create the two Tasks. They are automatically added to the task list
// and started when the Scheduler is called.
Thread1 thread1;
Thread2 thread2;

int
main()
{
	// start scheduler
	modm::rtos::Scheduler::schedule();

	return 0;
}

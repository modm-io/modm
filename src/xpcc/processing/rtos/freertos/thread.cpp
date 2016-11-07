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

#include "../thread.hpp"

/*
 * FreeRTOS is a C library. Therefore it doesn't know anything about objects
 * and their this-pointers. To be able to use FreeRTOS from C++ we use a
 * simple trick:
 *
 * FreeRTOS threads must be C functions which get a single void-Pointer as
 * argument. To call C++ member functions we use the static member function
 * xpcc::rtos::Thread::wrapper() as a wrapper which gets the a pointer to
 * the corresponding thread as parameter.
 * To be able to call the right run()-function run() needs to be virtual. This
 * adds the overhead of an additional vtable per Thread but is the only portable
 * solution.
 * Casting member functions pointer to void-pointers and back might work but
 * is not guaranteed to.
 */

// ----------------------------------------------------------------------------
void
xpcc::rtos::Thread::wrapper(void *object)
{
	Thread* thread = reinterpret_cast<Thread *>(object);
	thread->run();
}

xpcc::rtos::Thread::~Thread()
{
	vTaskDelete(this->handle);
}

// ----------------------------------------------------------------------------
xpcc::rtos::Thread::Thread(uint32_t priority,
		uint16_t stackDepth,
		const char* name)
{
	xTaskCreate(
			&wrapper,
			(const signed char*) name,
			(stackDepth / 4) + 1,
			this,
			priority,
			&this->handle);
}

// ----------------------------------------------------------------------------
uint32_t
xpcc::rtos::Thread::getPriority() const
{
	return uxTaskPriorityGet(this->handle);
}

void
xpcc::rtos::Thread::setPriority(uint32_t priority)
{
	vTaskPrioritySet(this->handle, priority);
}

// ----------------------------------------------------------------------------
/*void
xpcc::rtos::Thread::suspend()
{
	vTaskSuspend(this->handle);
}

void
xpcc::rtos::Thread::resume()
{
	vTaskResume(this->handle);
}

void
xpcc::rtos::Thread::resumeFromInterrupt()
{
	xTaskResumeFromISR(this->handle);
}*/


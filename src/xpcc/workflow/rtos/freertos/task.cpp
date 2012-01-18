// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#include "task.hpp"

/*
 * FreeRTOS is a C library. Therefore it doesn't know anything about objects
 * and their this-pointers. To be able to use FreeRTOS from C++ we use a
 * simple trick:
 *
 * FreeRTOS tasks must be C functions which get a single void-Pointer as
 * argument. To call C++ member functions we use the static member function
 * xpcc::rtos::Task::wrapper() as a wrapper which gets the a pointer to
 * the corresponding task as parameter.
 * To be able to call the right run()-function run() needs to be virtual. This
 * adds the overhead of an additional vtable per Task but is the only portable
 * solution.
 * Casting member functions pointer to void-pointers and back might work but
 * is not guaranteed to.
 */

// ----------------------------------------------------------------------------
void
xpcc::rtos::Task::wrapper(void *object)
{
	Task* task = reinterpret_cast<Task *>(object);
	task->run();
}

xpcc::rtos::Task::~Task()
{
	vTaskDelete(this->handle);
}

// ----------------------------------------------------------------------------
xpcc::rtos::Task::Task(unsigned portBASE_TYPE priority,
		unsigned short stackDepth,
		const char* name)
{
	xTaskCreate(
			&wrapper,
			(const signed char*) name,
			stackDepth,
			this,
			priority,
			&this->handle);
}

// ----------------------------------------------------------------------------
unsigned portBASE_TYPE
xpcc::rtos::Task::getPriority() const
{
	return uxTaskPriorityGet(this->handle);
}

void
xpcc::rtos::Task::setPriority(unsigned portBASE_TYPE priority)
{
	vTaskPrioritySet(this->handle, priority);
}

// ----------------------------------------------------------------------------
void
xpcc::rtos::Task::suspend()
{
	vTaskSuspend(this->handle);
}

void
xpcc::rtos::Task::resume()
{
	vTaskResume(this->handle);
}

void
xpcc::rtos::Task::resumeFromInterrupt()
{
	xTaskResumeFromISR(this->handle);
}

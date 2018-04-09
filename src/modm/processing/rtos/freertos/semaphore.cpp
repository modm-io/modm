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

// ----------------------------------------------------------------------------
modm::rtos::SemaphoreBase::~SemaphoreBase()
{
	// As semaphores are based on queues we use the queue functions to delete
	// the semaphore
	vQueueDelete(this->handle);
}

// ----------------------------------------------------------------------------
bool
modm::rtos::SemaphoreBase::acquire(portTickType timeout)
{

	return (xSemaphoreTake(this->handle, timeout) == pdTRUE);
}

void
modm::rtos::SemaphoreBase::release()
{
	xSemaphoreGive(this->handle);
}

void
modm::rtos::SemaphoreBase::releaseFromInterrupt()
{
	portBASE_TYPE threadWoken = pdFALSE;

	xSemaphoreGiveFromISR(this->handle, &threadWoken);

	// Request a context switch when the IRQ ends if a higher priorty has
	// been woken.
	portEND_SWITCHING_ISR(threadWoken);
}

// ----------------------------------------------------------------------------
modm::rtos::Semaphore::Semaphore(unsigned portBASE_TYPE max,
		unsigned portBASE_TYPE initial)
{
	this->handle = xSemaphoreCreateCounting(max, initial);
}

// ----------------------------------------------------------------------------
modm::rtos::BinarySemaphore::BinarySemaphore()
{
	vSemaphoreCreateBinary(this->handle);
}


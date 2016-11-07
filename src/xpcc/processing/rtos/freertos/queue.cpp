/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../queue.hpp"

// ----------------------------------------------------------------------------
xpcc::rtos::QueueBase::QueueBase(unsigned portBASE_TYPE length,
		unsigned portBASE_TYPE itemSize)
{
	this->handle = xQueueCreate(length, itemSize);
}

xpcc::rtos::QueueBase::~QueueBase()
{
	vQueueDelete(this->handle);
}

// ----------------------------------------------------------------------------
bool
xpcc::rtos::QueueBase::append(const void *item, portTickType timeout)
{
	return (xQueueSendToBack(this->handle, item, timeout) == pdTRUE);
}

bool
xpcc::rtos::QueueBase::prepend(const void *item, portTickType timeout)
{
	return (xQueueSendToFront(this->handle, item, timeout) == pdTRUE);
}

bool
xpcc::rtos::QueueBase::peek(void *item, portTickType timeout) const
{
	return (xQueuePeek(this->handle, item, timeout) == pdTRUE);
}

bool
xpcc::rtos::QueueBase::get(void *item, portTickType timeout)
{
	return (xQueueReceive(this->handle, item, timeout) == pdTRUE);
}

bool
xpcc::rtos::QueueBase::appendFromInterrupt(const void *item)
{
	portBASE_TYPE threadWoken = pdFALSE;
	portBASE_TYPE result = xQueueSendToBackFromISR(this->handle, item, &threadWoken);
	
	// Request a context switch when the IRQ ends if a higher priorty has
	// been woken.
	portEND_SWITCHING_ISR(threadWoken);
	
	return (result == pdPASS);
}

bool
xpcc::rtos::QueueBase::prependFromInterrupt(const void *item)
{
	portBASE_TYPE threadWoken = pdFALSE;
	portBASE_TYPE result = xQueueSendToFrontFromISR(this->handle, item, &threadWoken);
	
	// Request a context switch when the IRQ ends if a higher priorty has
	// been woken.
	portEND_SWITCHING_ISR(threadWoken);
	
	return (result == pdPASS);
}

bool
xpcc::rtos::QueueBase::getFromInterrupt(void *item)
{
	portBASE_TYPE threadWoken = pdFALSE;
	portBASE_TYPE result = xQueueReceiveFromISR(this->handle, item, &threadWoken);
	
	// Request a context switch when the IRQ ends if a higher priorty has
	// been woken.
	portEND_SWITCHING_ISR(threadWoken);
	
	return (result == pdTRUE);
}


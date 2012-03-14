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
 */
// ----------------------------------------------------------------------------

#include "semaphore.hpp"

// ----------------------------------------------------------------------------
xpcc::rtos::SemaphoreBase::~SemaphoreBase()
{
	// As semaphores are based on queues we use the queue functions to delete
	// the semaphore
	vQueueDelete(this->handle);
}

// ----------------------------------------------------------------------------
bool
xpcc::rtos::SemaphoreBase::acquire(portTickType timeout)
{
	
	return (xSemaphoreTake(this->handle, timeout) == pdTRUE);
}

void
xpcc::rtos::SemaphoreBase::release()
{
	xSemaphoreGive(this->handle);
}

void
xpcc::rtos::SemaphoreBase::releaseFromInterrupt()
{
	portBASE_TYPE threadWoken = pdFALSE;
	
	xSemaphoreGiveFromISR(this->handle, &threadWoken);
	
	// Request a context switch when the IRQ ends if a higher priorty has
	// been woken.
	portEND_SWITCHING_ISR(threadWoken);
}

// ----------------------------------------------------------------------------
xpcc::rtos::Semaphore::Semaphore(unsigned portBASE_TYPE max,
		unsigned portBASE_TYPE initial)
{
	this->handle = xSemaphoreCreateCounting(max, initial);
}

// ----------------------------------------------------------------------------
xpcc::rtos::BinarySemaphore::BinarySemaphore()
{
	vSemaphoreCreateBinary(this->handle);
}


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

xpcc::rtos::Task* xpcc::rtos::Task::head = 0;

// ----------------------------------------------------------------------------
xpcc::rtos::Task::Task(uint32_t priority, uint32_t stackDepth, const char* name) :
	next(0)
{
	// avoid compiler warnings
	(void) priority;
	(void) stackDepth;
	(void) name;
	
	// create a list of all threads
	if (head == 0) {
		head = this;
	}
	else {
		Task *list = head;
		while (list->next != 0) {
			list = list->next;
		}
		list->next = this;
	}
}

xpcc::rtos::Task::~Task()
{
}

// ----------------------------------------------------------------------------
void
xpcc::rtos::Task::suspend()
{
	// delete the current thread 
	this->thread.reset();
}

void
xpcc::rtos::Task::resume()
{
	this->thread.reset(new boost::thread(boost::bind(&Task::run, this)));
}

void
xpcc::rtos::Task::resumeFromInterrupt()
{
	this->resume();
}

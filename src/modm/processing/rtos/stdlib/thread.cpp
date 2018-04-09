/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2012, 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "thread.hpp"

modm::rtos::Thread* modm::rtos::Thread::head = 0;

// ----------------------------------------------------------------------------
modm::rtos::Thread::Thread(uint32_t priority, uint16_t stackDepth, const char* name) :
	next(0),
	thread()
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
		Thread *list = head;
		while (list->next != 0) {
			list = list->next;
		}
		list->next = this;
	}
}

modm::rtos::Thread::~Thread()
{
}

// ----------------------------------------------------------------------------
void
modm::rtos::Thread::start()
{
	this->thread.reset(new std::thread(&Thread::run, this));
}

/*
 * Copyright (c) 2009, Georgi Grinshpun
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

#include "../scheduler.hpp"
#include "../thread.hpp"

void
modm::rtos::Scheduler::schedule()
{
	// Start all threads
	Thread* list = Thread::head;
	while (list != 0) {
		list->start();
		list = list->next;
	}
	
	while (1)
	{
		// Threads are started and will do all the work. Just
		// sleep a bit here when there is nothing else to do. 
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

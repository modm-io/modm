/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "scheduler.hpp"

// ----------------------------------------------------------------------------
xpcc::Scheduler::Scheduler() :
	taskList(0), readyList(0), currentPriority(0)
{
}

// ----------------------------------------------------------------------------
void
xpcc::Scheduler::scheduleTask(Task& task,
		uint16_t period,
		Priority priority)
{
	TaskListItem *item = new TaskListItem(task, period, priority);
	
	if (taskList == 0) {
		taskList = item;
	}
	else {
		item->nextTask = taskList;
		taskList = item;
	}
}

// ----------------------------------------------------------------------------
/*bool
xpcc::Scheduler::removeTask(const Task& task)
{
}
*/

// ----------------------------------------------------------------------------
void
xpcc::Scheduler::schedule()
{
	this->scheduleInterupt();
}

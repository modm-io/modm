/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SCHEDULER_HPP
	#error	"Don't include this file directly, use 'scheduler.hpp' instead!"
#endif

/* item is element of two lists (schedule list and ready list).
 * ready list is order by its priority.
 * 
 * ALGORITHM:
 * ----------------------------------------------------------------------------
 * foreach item
 *     decrement
 *     if time = 0
 *         reload time
 *         set as ready
 * 
 * foreach item is ready (ordered by priority)
 *     run item
 *     mark as waiting
 * ----------------------------------------------------------------------------
 */
inline void
xpcc::Scheduler::scheduleInterupt()
{
	if (taskList == 0) {
		// nothing to schedule right now
		return;
	}
	
	// update all tasks
	TaskListItem *item = taskList;
	do {
		item->time--;
		if (item->time == 0) {
			item->time = item->period;
			
			// add to ready list
			if ((readyList == 0) ||
				(readyList->priority < item->priority))
			{
				item->nextReady = readyList;
				readyList = item;
			}
			else {
				TaskListItem *list = readyList;
				
				while (1)
				{
					if ((list->nextReady == 0) ||
						(list->nextReady->priority < item->priority))
					{
						item->nextReady = list->nextReady;
						list->nextReady = item;
						break;
					}
					list = list->nextReady;
				}
			}
			item->state = TaskListItem::READY;
		}
	}
	while ((item = item->nextTask) != 0);
	
	// now execute the tasks which are ready
	while (((item = xpcc::accessor::asVolatile(readyList)) != 0) &&
			(item->priority > currentPriority))
	{
		item->state = TaskListItem::RUNNING;
		readyList = item->nextReady;
		currentPriority = item->priority;
		{
			xpcc::atomic::Unlock();
			
			// the actual execution of the task happens with interrupts
			// enabled
			item->task.run();
		}
		currentPriority = 0;
		item->state = TaskListItem::WAITING;
	}
}

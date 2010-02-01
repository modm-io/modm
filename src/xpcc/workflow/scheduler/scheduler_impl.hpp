// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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
 * $Id: scheduler.hpp 95 2009-10-19 21:39:26Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SCHEDULER_HPP
	#error	"Don't include this file directly, use 'scheduler.hpp' instead!"
#endif

inline void
xpcc::Scheduler::scheduleInterupt()
{
	/* item is element of two lists (schedule list and ready list)
	   ready list is order after priority
	
	foreach item
		decrement
		if time = 0
			reload time
			set as ready
	
	foreach item is ready orderd per priority
		run item
		mark as waiting
	*/
	
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
	
	// how execute the tasks which are ready
	while (((item = xpcc::modifier::asVolatile(readyList)) != 0) &&
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

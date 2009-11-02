
#include "scheduler.hpp"

using namespace xpcc;

Scheduler::Scheduler() :
	taskList(0), readyList(0)
{
}

void
Scheduler::scheduleTask(Task& task,
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

bool
Scheduler::removeTask(const Task& /*task*/)
{
	return false;
}

void
Scheduler::addToReadyList(TaskListItem* item)
{
	if (readyList == 0)
	{
		readyList = item;
		item->state = TaskListItem::READY;
	}
	else {
		// TODO
	}
}

void
Scheduler::update()
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
		// nothing to shedule right now
		return;
	}
	
	TaskListItem *item = taskList;
	do {
		item->time--;
		if (item->time == 0) {
			item->time = item->period;	// reload time
			
			// add to ready list
			this->addToReadyList(item);
		}
	}
	while ((item = item->nextTask));
	
	while ((item = readyList))
	{
		item->state = TaskListItem::RUNNING;
		item->task.run();
		item->state = TaskListItem::WAITING;
		
		readyList = item->nextReady;
	}
}

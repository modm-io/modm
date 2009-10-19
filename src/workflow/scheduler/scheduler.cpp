
#include "scheduler.hpp"

void
xpcc::Scheduler::scheduleEvent(Event& /*event*/,
							   uint16_t /*period*/,
							   Priority /*priority*/)
{
	
}

bool
xpcc::Scheduler::removeEvent(const Event& /*event*/)
{
	return false;
}

void
xpcc::Scheduler::update()
{
	// item is element of two lists (schedule list and ready list)
	// ready list is order after priority
	/*
	foreach item
		decrement
		if time = 0
			set as ready
			reload time
	
	foreach item is ready orderd per priority
		run item
		mark as waiting
	*/
}

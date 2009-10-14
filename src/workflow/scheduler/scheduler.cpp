
#include "scheduler.hpp"

xpcc::Scheduler::EventId
xpcc::Scheduler::scheduleEvent(Event& /*event*/,
							   uint16_t /*period*/,
							   Priority /*priority*/)
{
	return 0;
}

bool
xpcc::Scheduler::removeEvent(EventId /*identifier*/)
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

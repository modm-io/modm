/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2015-2016, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SCHEDULER_HPP
#define XPCC__SCHEDULER_HPP

#include <stdint.h>

#include <modm/architecture/utils.hpp>
#include <modm/architecture/driver/accessor.hpp>
#include <modm/architecture/driver/atomic/lock.hpp>		// for Scheduler::scheduleInterrupt()

namespace xpcc
{
	/**
	 * \ingroup	processing
	 * \brief	%Scheduler
	 *
	 * If the schedule() method is call from a timer interrupt, this scheduler
	 * is a priority based preemptive scheduler, meaning that always the task
	 * with the highest priority is executed. It will only change tasks if a
	 * task with a higher priority becomes ready or the current task ends.
	 *
	 * \image	html	scheduler.png
	 *
	 * \warning	Works for ATmega, but currently not for the ATxmega!
	 *
	 * \author	Fabian Greif
	 * \todo	Check that this implementation works from inside an interrupt
	 */
	class Scheduler
	{
	public:
		typedef uint8_t Priority;

		/**
		 * \brief	%Scheduler task
		 */
		class Task
		{
		public:
			virtual void
			run() = 0;
		};

	public:
		Scheduler();

		void
		scheduleTask(Task& task,
					 uint16_t period,
					 Priority priority = 127);

		// TODO	Implement this function
		/*bool
		removeTask(const Task& task);*/

		void
		schedule();

		xpcc_always_inline void
		scheduleInterupt();

	private:
		struct TaskListItem
		{
			TaskListItem(Task& task,
						 uint16_t period,
						 Priority priority) :
				nextTask(0), nextReady(0), task(task),
				period(period), time(period), priority(priority),
				state(WAITING)
			{
			}

			TaskListItem *nextTask;
			TaskListItem *nextReady;

			Task& task;
			uint16_t period;
			uint16_t time;
			Priority priority;
			/// @cond
			enum {
				RUNNING,
				READY,
				WAITING
			} state;
			/// @endcond
		};

		TaskListItem *taskList;
		TaskListItem *readyList;

		Priority currentPriority;
	};
}

#include "scheduler_impl.hpp"

#endif // XPCC__SCHEDULER_HPP

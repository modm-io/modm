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
#define XPCC__SCHEDULER_HPP

#include <stdint.h>
#include <xpcc/utils/misc.hpp>

namespace xpcc
{
	/**
	 * @ingroup	workflow
	 * @brief	%Scheduler
	 * 
	 * If the schedule() method is call from a timer interrupt, this scheduler
	 * is a priority based preemtive scheduler, meaning that always the task
	 * with the highest priority is executed. It will only change tasks if a
	 * task with a higher priority becomes ready or the current task ends.
	 * 
	 * @image	html	scheduler.png
	 * 
	 * 
	 * @todo	Check that this implementation works from inside an interrupt
	 */
	class Scheduler
	{
	public:
		typedef uint8_t Priority;
		
		/// @brief	%Scheduler task
		class Task
		{
		public:
			virtual void
			run() = 0;
		};
	
	public:
		Scheduler();
		
		/// @brief	
		void
		scheduleTask(Task& task,
					 uint16_t period,
					 Priority priority = 127);
		
		/// @todo	implement this function
		bool
		removeTask(const Task& task);
		
		void
		schedule();
		
		ALWAYS_INLINE void
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
			enum {
				RUNNING,
				READY,
				WAITING
			} state;
		};
		
		TaskListItem *taskList;
		TaskListItem *readyList;
		
		Priority currentPriority;
	};
}

#include "scheduler_impl.hpp"

#endif // XPCC__SCHEDULER_HPP

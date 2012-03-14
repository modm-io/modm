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
 */
// ----------------------------------------------------------------------------

#include <xpcc/workflow/scheduler/scheduler.hpp>

#include "scheduler_test.hpp"

// ----------------------------------------------------------------------------

static unsigned int count = 1;

class TestTask : public xpcc::Scheduler::Task
{
public:
	TestTask() :
		order(0)
	{
	}
	
	virtual void
	run()
	{
		order = count;
		count++;
	}
	
	uint8_t order;
};

// ----------------------------------------------------------------------------

void
SchedulerTest::testScheduler()
{
	xpcc::Scheduler scheduler;
	
	TestTask task1;
	TestTask task2;
	TestTask task3;
	TestTask task4;
	
	scheduler.scheduleTask(task1, 3, 10);
	scheduler.scheduleTask(task2, 3);
	scheduler.scheduleTask(task3, 3, 20);
	scheduler.scheduleTask(task4, 3, 200);
	
	scheduler.schedule();
	scheduler.schedule();
	
	TEST_ASSERT_EQUALS(task1.order, 0);
	TEST_ASSERT_EQUALS(task2.order, 0);
	TEST_ASSERT_EQUALS(task3.order, 0);
	TEST_ASSERT_EQUALS(task4.order, 0);
	
	scheduler.schedule();
	
	TEST_ASSERT_EQUALS(task1.order, 4);
	TEST_ASSERT_EQUALS(task2.order, 2);
	TEST_ASSERT_EQUALS(task3.order, 3);
	TEST_ASSERT_EQUALS(task4.order, 1);
	
	count = 1;
	task1.order = 0;
	task2.order = 0;
	task3.order = 0;
	task4.order = 0;
	
	scheduler.schedule();
	scheduler.schedule();
	
	TEST_ASSERT_EQUALS(task1.order, 0);
	TEST_ASSERT_EQUALS(task2.order, 0);
	TEST_ASSERT_EQUALS(task3.order, 0);
	TEST_ASSERT_EQUALS(task4.order, 0);
	
	scheduler.schedule();
	
	TEST_ASSERT_EQUALS(task1.order, 4);
	TEST_ASSERT_EQUALS(task2.order, 2);
	TEST_ASSERT_EQUALS(task3.order, 3);
	TEST_ASSERT_EQUALS(task4.order, 1);
}

/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/processing/scheduler/scheduler.hpp>

#include "scheduler_test.hpp"

// ----------------------------------------------------------------------------

static unsigned int count = 1;

class TestTask : public modm::Scheduler::Task
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
	modm::Scheduler scheduler;
	
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

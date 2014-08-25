// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/processing/protothread.hpp>
#include <iostream>

#include "nested_protothread_test.hpp"

namespace Task
{
	enum
	{
		Empty0Task1,
		Empty0Task2,
		Empty1Task1,
		Empty1Task2,
		Empty2Task1 = Empty1Task1,
		Empty2Task2 = Empty1Task2
	};
}

// ----------------------------------------------------------------------------
class TestingEmptyThread0 : public xpcc::pt::NestedProtothread<0>
{
public:
	TestingEmptyThread0()
	:	state(0), depth(0)
	{
	}

	bool
	runTask1()
	{
		NPT_BEGIN(Task::Empty0Task1);

		state = 1;
		depth = getTaskDepth();

		NPT_YIELD();

		state = 2;

		NPT_END();
	}

	bool
	runTask2()
	{
		NPT_BEGIN(Task::Empty0Task2);

		state = 3;

		NPT_END();
	}

	uint8_t state;
	int8_t depth;
};

class TestingEmptyThread1 : public xpcc::pt::NestedProtothread<1>
{
public:
	TestingEmptyThread1()
	:	state(0)
	{
	}

	bool
	runTask1()
	{
		NPT_BEGIN(Task::Empty1Task1);

		state = 1;

		NPT_YIELD();

		state = 2;

		NPT_END();
	}

	bool
	runTask2()
	{
		NPT_BEGIN(Task::Empty1Task2);

		state = 3;

		NPT_END();
	}

	uint8_t state;
};

class TestingEmptyThread2 : public xpcc::pt::NestedProtothread<2>
{
public:
	TestingEmptyThread2()
	:	state(0)
	{
	}

	bool
	runTask1()
	{
		NPT_BEGIN(Task::Empty2Task1);

		state = 1;

		NPT_YIELD();

		state = 2;

		NPT_END();
	}

	bool
	runTask2()
	{
		NPT_BEGIN(Task::Empty2Task2);

		state = 3;

		NPT_END();
	}

	uint8_t state;
};

void
NestedProtothreadTest::testClassMethods()
{
	// Nesting Depth of zero is specialized
	TestingEmptyThread0 thread0;
	// these threads don't start themselves
	TEST_ASSERT_FALSE(thread0.isTaskRunning());
	TEST_ASSERT_EQUALS(thread0.getTaskDepth(), -1);
	// calling run() should return false
	TEST_ASSERT_FALSE(thread0.runTask1());
	TEST_ASSERT_FALSE(thread0.runTask2());
	// calling run() should not have modified anything
	TEST_ASSERT_EQUALS(thread0.state, 0);
	TEST_ASSERT_EQUALS(thread0.depth, 0);
	// still not running
	TEST_ASSERT_FALSE(thread0.isTaskRunning());

	// lets start a task
	TEST_ASSERT_TRUE(thread0.startTask(Task::Empty0Task1));
	// now it should be running
	TEST_ASSERT_TRUE(thread0.isTaskRunning());
	// while a task is running, another cannot be started
	TEST_ASSERT_FALSE(thread0.startTask(Task::Empty0Task2));
	// the thread should yield once
	TEST_ASSERT_TRUE(thread0.runTask1());
	// state should be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// running the other task should not change anything
	TEST_ASSERT_FALSE(thread0.runTask2());
	// state should be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// the thread should conclude
	TEST_ASSERT_FALSE(thread0.runTask1());
	// state should be 2
	TEST_ASSERT_EQUALS(thread0.state, 2);

	// not running anymore
	TEST_ASSERT_FALSE(thread0.isTaskRunning());

	// after finishing a task should not run again
	thread0.state = 0;
	// the thread should end immidiately
	TEST_ASSERT_FALSE(thread0.runTask1());
	TEST_ASSERT_FALSE(thread0.runTask2());
	// state should be 0
	TEST_ASSERT_EQUALS(thread0.state, 0);

	// try the same with task2
	TEST_ASSERT_TRUE(thread0.startTask(Task::Empty0Task2));
	// now it should be running
	TEST_ASSERT_TRUE(thread0.isTaskRunning());
	// again, another task cannot be started (not even the same task)
	TEST_ASSERT_FALSE(thread0.startTask(Task::Empty0Task2));
	// the thread should end immidiately
	TEST_ASSERT_FALSE(thread0.runTask2());
	TEST_ASSERT_FALSE(thread0.runTask1());
	// state should be 3
	TEST_ASSERT_EQUALS(thread0.state, 3);
	// not running anymore
	TEST_ASSERT_FALSE(thread0.isTaskRunning());

	// test the same thing with the generic implementation
	TestingEmptyThread1 thread1;
	TEST_ASSERT_FALSE(thread1.isTaskRunning());
	TEST_ASSERT_FALSE(thread1.runTask1());
	TEST_ASSERT_FALSE(thread1.runTask2());
	TEST_ASSERT_EQUALS(thread1.state, 0);
	TEST_ASSERT_FALSE(thread1.isTaskRunning());

	TEST_ASSERT_TRUE(thread1.startTask(Task::Empty1Task1));
	TEST_ASSERT_TRUE(thread1.isTaskRunning());
	TEST_ASSERT_FALSE(thread1.startTask(Task::Empty1Task2));
	TEST_ASSERT_TRUE(thread1.runTask1());
	TEST_ASSERT_EQUALS(thread1.state, 1);
	TEST_ASSERT_FALSE(thread1.runTask2());
	TEST_ASSERT_EQUALS(thread1.state, 1);
	TEST_ASSERT_FALSE(thread1.runTask1());
	TEST_ASSERT_EQUALS(thread1.state, 2);
	TEST_ASSERT_FALSE(thread1.isTaskRunning());

	thread1.state = 0;
	TEST_ASSERT_FALSE(thread1.runTask1());
	TEST_ASSERT_FALSE(thread1.runTask2());
	TEST_ASSERT_EQUALS(thread1.state, 0);

	TEST_ASSERT_TRUE(thread1.startTask(Task::Empty1Task2));
	TEST_ASSERT_TRUE(thread1.isTaskRunning());
	TEST_ASSERT_FALSE(thread1.startTask(Task::Empty1Task2));
	TEST_ASSERT_FALSE(thread1.runTask2());
	TEST_ASSERT_FALSE(thread1.runTask1());
	TEST_ASSERT_EQUALS(thread1.state, 3);
	TEST_ASSERT_FALSE(thread1.isTaskRunning());

	// thread 2
	TestingEmptyThread2 thread2;
	TEST_ASSERT_FALSE(thread2.isTaskRunning());
	TEST_ASSERT_FALSE(thread2.runTask1());
	TEST_ASSERT_FALSE(thread2.runTask2());
	TEST_ASSERT_EQUALS(thread2.state, 0);
	TEST_ASSERT_FALSE(thread2.isTaskRunning());

	TEST_ASSERT_TRUE(thread2.startTask(Task::Empty2Task1));
	TEST_ASSERT_TRUE(thread2.isTaskRunning());
	TEST_ASSERT_FALSE(thread2.startTask(Task::Empty2Task2));
	TEST_ASSERT_TRUE(thread2.runTask1());
	TEST_ASSERT_EQUALS(thread2.state, 1);
	TEST_ASSERT_FALSE(thread2.runTask2());
	TEST_ASSERT_EQUALS(thread2.state, 1);
	TEST_ASSERT_FALSE(thread2.runTask1());
	TEST_ASSERT_EQUALS(thread2.state, 2);
	TEST_ASSERT_FALSE(thread2.isTaskRunning());

	thread2.state = 0;
	TEST_ASSERT_FALSE(thread2.runTask1());
	TEST_ASSERT_FALSE(thread2.runTask2());
	TEST_ASSERT_EQUALS(thread2.state, 0);

	TEST_ASSERT_TRUE(thread2.startTask(Task::Empty2Task2));
	TEST_ASSERT_TRUE(thread2.isTaskRunning());
	TEST_ASSERT_FALSE(thread2.startTask(Task::Empty2Task2));
	TEST_ASSERT_FALSE(thread2.runTask2());
	TEST_ASSERT_FALSE(thread2.runTask1());
	TEST_ASSERT_EQUALS(thread2.state, 3);
	TEST_ASSERT_FALSE(thread2.isTaskRunning());
}

namespace Task
{
	enum
	{
		NestingTask1,
		NestingTask2,
		NestingTask3,
		NestingTask4
	};
}

// ----------------------------------------------------------------------------
class TestingNestedThread : public xpcc::pt::NestedProtothread<2>
{
	friend class xpcc::pt::NestedProtothread<2>;
public:
	TestingNestedThread()
	:	depth1(0), depth2(0), depth3(0),
		state1(0), state2(0), state3(0),
		condition1(false), condition2(false), condition3(false),
		callResult1(false), callResult2(false), callResult3(false),
		callResult1Retry(false), callResult1AfterStop(false),
		callResult3Retry(false), callResult3AfterStop(false)
	{
	}

	bool
	startTask1()
	{ return startTask(Task::NestingTask1); }

	bool
	runTask1()
	{
		NPT_BEGIN(Task::NestingTask1);

		state1 = 1;

		NPT_WAIT_UNTIL(condition1);

		state1 = 2;
		callResult1 = startTask(Task::NestingTask2);
		callResult1Retry = startTask(Task::NestingTask2);
		stopTask();
		callResult1AfterStop = startTask(Task::NestingTask2);
		depth1 = getTaskDepth();

		NPT_YIELD();

		NPT_WAIT_WHILE(runTask2());

		state1 = 3;

		NPT_YIELD();

		state1 = 4;

		NPT_END();
	}

//	void
//	printStateArray()
//	{
//		for (uint8_t ii = 0; ii < 2; ii++)
//		{
//			std::cout << nptStateArray[ii] << " ";
//		}
//		std::cout << std::endl;
//	}

protected:
	bool
	runTask2()
	{
		NPT_BEGIN(Task::NestingTask2);

		state2 = 1;

		NPT_WAIT_UNTIL(condition2);

		state2 = 2;
		callResult2 = startTask(Task::NestingTask3);
		depth2 = getTaskDepth();

		NPT_YIELD();

		NPT_WAIT_WHILE(runTask3());

		state2 = 3;

		NPT_YIELD();

		state2 = 4;

		NPT_END();
	}

	bool
	runTask3()
	{
		NPT_BEGIN(Task::NestingTask3);

		state3 = 1;

		NPT_WAIT_UNTIL(condition3);

		state3 = 2;
		// this must return false, since there is no more space
		// to buffer the next nested local continuation anymore!
		callResult3 = startTask(Task::NestingTask4);
		callResult3Retry = startTask(Task::NestingTask4);
		stopTask();
		callResult3AfterStop = startTask(Task::NestingTask4);
		depth3 = getTaskDepth();

		NPT_YIELD();

		state3 = 3;

		NPT_YIELD();

		state3 = 4;

		NPT_END();
	}

public:
	int8_t depth1;
	int8_t depth2;
	int8_t depth3;
	uint8_t state1;
	uint8_t state2;
	uint8_t state3;
	bool condition1;
	bool condition2;
	bool condition3;
	bool callResult1;
	bool callResult2;
	bool callResult3;
	bool callResult1Retry;
	bool callResult1AfterStop;
	bool callResult3Retry;
	bool callResult3AfterStop;
};

void
NestedProtothreadTest::testNesting()
{
	TestingNestedThread thread;

	// sanity checks
	TEST_ASSERT_FALSE(thread.isTaskRunning());
	TEST_ASSERT_FALSE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 0);
	TEST_ASSERT_FALSE(thread.isTaskRunning());
	TEST_ASSERT_EQUALS(thread.getTaskDepth(), -1);

	// lets start the first task
	TEST_ASSERT_TRUE(thread.startTask(Task::NestingTask1));
	// should wait until the first condition
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 1);
	// task should wait here
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 1);

	thread.condition1 = true;
	// task should continue
	TEST_ASSERT_TRUE(thread.runTask1());
	// task should continue
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 0);
	// the callResult1 should be true
	TEST_ASSERT_TRUE(thread.callResult1);
	// retry should be false
	TEST_ASSERT_FALSE(thread.callResult1Retry);
	// retry after stop should be true
	TEST_ASSERT_TRUE(thread.callResult1AfterStop);
	// depth should be 0
	TEST_ASSERT_EQUALS(thread.depth1, 0);

	// task2 should be called internally now
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 1);
	// should wait until the second condition
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 1);
	// task should wait here
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 1);

	thread.condition2 = true;
	// task1 should continue and task3 started
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	// the callResult2 should be true
	TEST_ASSERT_TRUE(thread.callResult2);
	// depth should be 1
	TEST_ASSERT_EQUALS(thread.depth2, 1);

	// task3 should be called internally now
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 1);
	// should wait until the second condition
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 1);
	// task should wait here
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 1);

	thread.condition3 = true;
	// task1 should continue and task4 not started
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 2);
	// the callResult3 should be false
	TEST_ASSERT_FALSE(thread.callResult3);
	// retry should be false
	TEST_ASSERT_FALSE(thread.callResult3Retry);
	// retry after stop should still be false
	TEST_ASSERT_FALSE(thread.callResult3AfterStop);
	// depth should be 2
	TEST_ASSERT_EQUALS(thread.depth3, 2);

	// task1 should continue
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 3);

	// task3 should end now
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 3);
	TEST_ASSERT_EQUALS(thread.state3, 4);

	// task2 should end now
	TEST_ASSERT_TRUE(thread.runTask1());
	TEST_ASSERT_EQUALS(thread.state1, 3);
	TEST_ASSERT_EQUALS(thread.state2, 4);
	TEST_ASSERT_EQUALS(thread.state3, 4);

	// task1 should end now
	TEST_ASSERT_FALSE(thread.runTask1());
	TEST_ASSERT_FALSE(thread.isTaskRunning());
}

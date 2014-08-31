// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/processing/protothread.hpp>
#include "nested_protothread_test.hpp"

// ----------------------------------------------------------------------------
class TestingEmptyThread0 : public xpcc::pt::NestedProtothread<0>
{
public:
	TestingEmptyThread0()
	:	state(0), depth(0)
	{
	}

	xpcc::pt::Result
	task1(void *ctx)
	{
		NPT_BEGIN(ctx);

		state = 1;
		depth = getTaskDepth();

		NPT_YIELD();

		state = 2;

		NPT_END();
	}

	xpcc::pt::Result
	task2(void *ctx)
	{
		NPT_BEGIN(ctx);

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
	:	state(0), depth(0)
	{
	}

	xpcc::pt::Result
	task1(void *ctx)
	{
		NPT_BEGIN(ctx);

		state = 1;
		depth = getTaskDepth();

		NPT_YIELD();

		state = 2;

		NPT_END();
	}

	xpcc::pt::Result
	task2(void *ctx)
	{
		NPT_BEGIN(ctx);

		state = 3;

		NPT_END();
	}

	uint8_t state;
	int8_t depth;
};

class TestingEmptyThread2 : public xpcc::pt::NestedProtothread<2>
{
public:
	TestingEmptyThread2()
	:	state(0), depth(0)
	{
	}

	xpcc::pt::Result
	task1(void *ctx)
	{
		NPT_BEGIN(ctx);

		state = 1;
		depth = getTaskDepth();

		NPT_YIELD();

		state = 2;

		NPT_END();
	}

	xpcc::pt::Result
	task2(void *ctx)
	{
		NPT_BEGIN(ctx);

		state = 3;

		NPT_END();
	}

	uint8_t state;
	int8_t depth;
};

void
NestedProtothreadTest::testClassMethods()
{
	uint8_t ctx_helper;
	void *ctx1 = this;
	void *ctx2 = &ctx_helper;

	// Nesting Depth of zero is specialized
	TestingEmptyThread0 thread0;
	// outside the nesting depth should be -1
	TEST_ASSERT_EQUALS(thread0.getTaskDepth(), -1);
	// nothing should have been modified
	TEST_ASSERT_EQUALS(thread0.state, 0);
	TEST_ASSERT_EQUALS(thread0.depth, 0);
	// still not running
	TEST_ASSERT_FALSE(thread0.isTaskRunning());

	// lets start a task, which will yield
	TEST_ASSERT_EQUALS(thread0.task1(ctx1), xpcc::pt::Running);
	// now it should be running
	TEST_ASSERT_TRUE(thread0.isTaskRunning());
	// state should be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// depth should be 0
	TEST_ASSERT_EQUALS(thread0.depth, 0);

	// two tasks cannot run in the same context
	TEST_ASSERT_EQUALS(thread0.task2(ctx1), xpcc::pt::WrongState);
	// but starting a task of the same class in another context
	// returns WrongContext, which can make parent class wait
	TEST_ASSERT_EQUALS(thread0.task2(ctx2), xpcc::pt::WrongContext);
	// Similarly calling a running task1 in another context
	// also returns WrongContext
	TEST_ASSERT_EQUALS(thread0.task1(ctx2), xpcc::pt::WrongContext);

	// state should still be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// but it should continue execution in the right context
	TEST_ASSERT_EQUALS(thread0.task1(ctx1), xpcc::pt::Stop);
	// state should be 2
	TEST_ASSERT_EQUALS(thread0.state, 2);
	// nothing is running anymore
	TEST_ASSERT_FALSE(thread0.isTaskRunning());

	// try the same with task2, which will end immediately
	TEST_ASSERT_EQUALS(thread0.task2(ctx1), xpcc::pt::Stop);
	// state should be 3
	TEST_ASSERT_EQUALS(thread0.state, 3);
	// not running anymore
	TEST_ASSERT_FALSE(thread0.isTaskRunning());


	// generic implementation with 1 nesting levels
	TestingEmptyThread1 thread1;
	TEST_ASSERT_EQUALS(thread1.getTaskDepth(), -1);
	TEST_ASSERT_EQUALS(thread1.state, 0);
	TEST_ASSERT_EQUALS(thread1.depth, 0);
	TEST_ASSERT_FALSE(thread1.isTaskRunning());

	TEST_ASSERT_EQUALS(thread1.task1(ctx1), xpcc::pt::Running);
	TEST_ASSERT_TRUE(thread1.isTaskRunning());
	TEST_ASSERT_EQUALS(thread1.state, 1);
	TEST_ASSERT_EQUALS(thread1.depth, 0);

	TEST_ASSERT_EQUALS(thread1.task2(ctx1), xpcc::pt::WrongState);
	TEST_ASSERT_EQUALS(thread1.task2(ctx2), xpcc::pt::WrongContext);
	TEST_ASSERT_EQUALS(thread1.task1(ctx2), xpcc::pt::WrongContext);

	TEST_ASSERT_EQUALS(thread1.state, 1);
	TEST_ASSERT_EQUALS(thread1.task1(ctx1), xpcc::pt::Stop);
	TEST_ASSERT_EQUALS(thread1.state, 2);
	TEST_ASSERT_FALSE(thread1.isTaskRunning());

	TEST_ASSERT_EQUALS(thread1.task2(ctx1), xpcc::pt::Stop);
	TEST_ASSERT_EQUALS(thread1.state, 3);
	TEST_ASSERT_FALSE(thread1.isTaskRunning());


	// generic implementation with 2 nesting levels
	TestingEmptyThread2 thread2;
	TEST_ASSERT_EQUALS(thread2.getTaskDepth(), -1);
	TEST_ASSERT_EQUALS(thread2.state, 0);
	TEST_ASSERT_EQUALS(thread2.depth, 0);
	TEST_ASSERT_FALSE(thread2.isTaskRunning());

	TEST_ASSERT_EQUALS(thread2.task1(ctx1), xpcc::pt::Running);
	TEST_ASSERT_TRUE(thread2.isTaskRunning());
	TEST_ASSERT_EQUALS(thread2.state, 1);
	TEST_ASSERT_EQUALS(thread2.depth, 0);

	TEST_ASSERT_EQUALS(thread2.task2(ctx1), xpcc::pt::WrongState);
	TEST_ASSERT_EQUALS(thread2.task2(ctx2), xpcc::pt::WrongContext);
	TEST_ASSERT_EQUALS(thread2.task1(ctx2), xpcc::pt::WrongContext);

	TEST_ASSERT_EQUALS(thread2.state, 1);
	TEST_ASSERT_EQUALS(thread2.task1(ctx1), xpcc::pt::Stop);
	TEST_ASSERT_EQUALS(thread2.state, 2);
	TEST_ASSERT_FALSE(thread2.isTaskRunning());

	TEST_ASSERT_EQUALS(thread2.task2(ctx1), xpcc::pt::Stop);
	TEST_ASSERT_EQUALS(thread2.state, 3);
	TEST_ASSERT_FALSE(thread2.isTaskRunning());
}


// ----------------------------------------------------------------------------
class TestingNestedThread : public xpcc::pt::NestedProtothread<2>
{
public:
	TestingNestedThread()
	:	depth1(0), depth2(0), depth3(0),
		state1(0), state2(0), state3(0),
		condition1(false), condition2(false), condition3(false),
		callResult1(0), callResult2(0), callResult3(0)
	{
	}

	xpcc::pt::Result
	task1(void *ctx)
	{
		NPT_BEGIN(ctx);

		state1 = 1;

		NPT_WAIT_UNTIL_START(condition1);

		state1 = 2;
		depth1 = getTaskDepth();

		NPT_YIELD();

		// manual spawn
		NPT_WAIT_WHILE((callResult1 = task2(ctx)) > xpcc::pt::Success);

		state1 = 3;

		NPT_YIELD();

		state1 = 4;

		NPT_END();
	}

protected:
	xpcc::pt::Result
	task2(void *ctx)
	{
		NPT_BEGIN(ctx);

		state2 = 1;

		NPT_WAIT_UNTIL_START(condition2);

		state2 = 2;
		depth2 = getTaskDepth();

		NPT_YIELD();

		NPT_WAIT_WHILE((callResult2 = task3(ctx)) > xpcc::pt::Success);

		state2 = 3;

		NPT_YIELD();

		state2 = 4;

		NPT_END();
	}

	xpcc::pt::Result
	task3(void *ctx)
	{
		NPT_BEGIN(ctx);

		state3 = 1;

		NPT_WAIT_UNTIL_START(condition3);

		state3 = 2;
		depth3 = getTaskDepth();
		// this must return NestingError, since there is no more space
		// to buffer the next nested local continuation anymore!
		callResult3 = task3(ctx);

		NPT_YIELD();

		state3 = 3;

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
	uint8_t callResult1;
	uint8_t callResult2;
	uint8_t callResult3;
};
//*/

void
NestedProtothreadTest::testNesting()
{
	uint8_t ctx_helper;
	void *ctx1 = this;
	void *ctx2 = &ctx_helper;

	TestingNestedThread thread;
	// sanity checks
	TEST_ASSERT_FALSE(thread.isTaskRunning());
	TEST_ASSERT_EQUALS(thread.state1, 0);
	TEST_ASSERT_EQUALS(thread.getTaskDepth(), -1);

	// should wait until the first condition
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Starting);
	TEST_ASSERT_EQUALS(thread.state1, 1);
	// task should wait here
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Starting);
	TEST_ASSERT_EQUALS(thread.state1, 1);

	// it should be running
	TEST_ASSERT_TRUE(thread.isTaskRunning());
	// we should be able to stop this task though
	thread.stopTask();
	TEST_ASSERT_FALSE(thread.isTaskRunning());
	// and restart it
	thread.state1 = 0;
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Starting);
	TEST_ASSERT_EQUALS(thread.state1, 1);

	// lets release start condition 1
	thread.condition1 = true;
	// task should continue and yield
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Running);
	// check the state and depth
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.depth1, 0);

	// first manual spawn
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Running);
	// the callResult1 should be Starting
	TEST_ASSERT_EQUALS(thread.callResult1, xpcc::pt::Starting);
	// after another run, callResult1 should still be Starting
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.callResult1, xpcc::pt::Starting);

	// task1 should reject any other contexts, even when currently spawning
	TEST_ASSERT_EQUALS(thread.task1(ctx2), xpcc::pt::WrongContext);

	// lets release start condition 2
	thread.condition2 = true;
	// task2 will progress to first yield
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Running);
	// callResult1 should be Running
	TEST_ASSERT_EQUALS(thread.callResult1, xpcc::pt::Running);
	// check the state and depth
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.depth2, 1);

	// task2 will progress to spawning task3
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Running);
	// callResult1 should be Running
	TEST_ASSERT_EQUALS(thread.callResult1, xpcc::pt::Running);
	// callResult2 should be Starting
	TEST_ASSERT_EQUALS(thread.callResult2, xpcc::pt::Starting);

	// after another run, this should not change
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.callResult1, xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.callResult2, xpcc::pt::Starting);

	// lets release start condition 3
	thread.condition3 = true;
	// task3 will progress to first yield
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.callResult1, xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.callResult2, xpcc::pt::Running);
	// check the states and depths
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 2);
	TEST_ASSERT_EQUALS(thread.depth3, 2);
	// we have exhausted the nesting capabilities
	TEST_ASSERT_EQUALS(thread.callResult3, xpcc::pt::NestingError);

	// now we will begin to strip down the nestings
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.callResult1, xpcc::pt::Running);
	// task3 will complete
	TEST_ASSERT_EQUALS(thread.state3, 3);
	// callResult2 will return Stop
	TEST_ASSERT_EQUALS(thread.callResult2, xpcc::pt::Stop);
	// task2 will continue until next yield
	TEST_ASSERT_EQUALS(thread.state2, 3);

	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Running);
	// task2 will complete
	TEST_ASSERT_EQUALS(thread.state2, 4);
	// callResult1 will return Stop
	TEST_ASSERT_EQUALS(thread.callResult1, xpcc::pt::Stop);
	// task1 will continure until next yield
	TEST_ASSERT_EQUALS(thread.state1, 3);

	// task1 should end now
	TEST_ASSERT_EQUALS(thread.task1(ctx1), xpcc::pt::Stop);
	TEST_ASSERT_EQUALS(thread.state1, 4);

	// nothing is running
	TEST_ASSERT_FALSE(thread.isTaskRunning());
}

uint8_t waits = 3;

class TestingSpawningThread : public xpcc::pt::NestedProtothread<1>
{
public:
	TestingSpawningThread()
	:	state(0)
	{
	}

	xpcc::pt::Result
	parentTask(void *ctx)
	{
		NPT_BEGIN(ctx);

		state = 1;
		NPT_YIELD();

		success = NPT_SPAWN(spawningTask(ctx, waits));

		state = 3;
		NPT_YIELD();

		if (success)
			state = 4;
		else
			state = 5;

		NPT_END();
	}

protected:
	xpcc::pt::Result
	spawningTask(void *ctx, uint8_t calls)
	{
		NPT_BEGIN(ctx);

		NPT_WAIT_WHILE_START(!startSpawningTask(calls));

		state = 2;
		NPT_YIELD();

		NPT_WAIT_WHILE(runSpawningTask(calls));

		NPT_SUCCESS_IF(isSpawningTaskSuccessful(calls));

		NPT_END();
	}

	bool
	startSpawningTask(uint8_t calls)
	{
		// manually implemented "protothread" without any side-effects on the NPT
		static uint8_t st_calls(0);

		if (st_calls++ < calls)
			return false;

		st_calls = 0;
		return true;
	}

	bool
	runSpawningTask(uint8_t calls)
	{
		// manually implemented "protothread" without any side-effects on the NPT
		static uint8_t st_calls(0);

		if (st_calls++ < calls)
			return true;

		st_calls = 0;
		return false;
	}

	bool
	isSpawningTaskSuccessful(uint8_t calls)
	{ return (calls == 2); }

public:
	uint8_t state;
	bool success;
};

void
NestedProtothreadTest::testSpawn()
{
	TestingSpawningThread thread;

	// sanity checks
	TEST_ASSERT_FALSE(thread.isTaskRunning());
	TEST_ASSERT_EQUALS(thread.state, 0);
	TEST_ASSERT_EQUALS(thread.getTaskDepth(), -1);

	// should wait until the first condition
	TEST_ASSERT_EQUALS(thread.parentTask(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state, 1);
	// task should require `waits` number of calls
	for (uint8_t ii = 0; ii < waits; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.parentTask(this), xpcc::pt::Running);
		TEST_ASSERT_EQUALS(thread.state, 1);
	}
	// now spawning task has started
	TEST_ASSERT_EQUALS(thread.parentTask(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state, 2);
	// task should require `waits` number of calls again
	for (uint8_t ii = 0; ii < waits; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.parentTask(this), xpcc::pt::Running);
		TEST_ASSERT_EQUALS(thread.state, 2);
	}
	// now spawning task has finished
	TEST_ASSERT_EQUALS(thread.parentTask(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state, 3);

	// now parent task has finished
	TEST_ASSERT_EQUALS(thread.parentTask(this), xpcc::pt::Stop);
	TEST_ASSERT_EQUALS(thread.state, (waits == 2) ? 4 : 5);
}


class TestingAbortingThread : public xpcc::pt::NestedProtothread<2>
{
public:
	TestingAbortingThread()
	:	state1(0), state2(0), state3(0),
		success1(false), success2(false), success3(false)
	{
	}

	xpcc::pt::Result
	task1(void *ctx)
	{
		static uint8_t calls = 3;
		NPT_BEGIN(ctx);

		NPT_WAIT_UNTIL_START(calls-- == 0);

		state1 = 1;
		NPT_YIELD();

		state1 = 2;
		success1 = NPT_SPAWN(task2(ctx));

		state1 = 3;
		NPT_YIELD();

		NPT_SUCCESS_IF(success1);

		state1 = 4;

		NPT_END_ON_ABORT(state1 = -1; calls = 3);
	}

protected:
	xpcc::pt::Result
	task2(void *ctx)
	{
		static uint8_t calls = 1;
		NPT_BEGIN(ctx);

		NPT_WAIT_UNTIL_START(calls-- == 0);

		state2 = 1;
		NPT_YIELD();

		state2 = 2;
		success2 = NPT_SPAWN(task3(ctx));

		state2 = 3;
		NPT_YIELD();

		NPT_SUCCESS_IF(success2);

		state2 = 4;

		NPT_END_ON_ABORT(
		{
			state2 = -1;
			calls = 1;
		});
	}

	xpcc::pt::Result
	task3(void *ctx)
	{
		static uint8_t calls = 1;
		NPT_BEGIN(ctx);

		NPT_WAIT_UNTIL_START(calls-- == 0);

		state3 = 1;
		NPT_YIELD();

		NPT_SUCCESS_IF(success3);

		state3 = 2;
		NPT_YIELD();

		NPT_ABORT();

		// no abort code here, but it should still work
		NPT_END_ON_ABORT(state3 = -1; calls = 1;);
	}

public:
	int8_t state1;
	int8_t state2;
	int8_t state3;
	bool success1;
	bool success2;
	bool success3;
};

class TestingAbortingThread0 : public xpcc::pt::NestedProtothread<0>
{
public:
	TestingAbortingThread0()
	:	state1(0), success1(false)
	{
	}

	xpcc::pt::Result
	task1(void *ctx)
	{
		static uint8_t calls = 3;
		NPT_BEGIN(ctx);

		NPT_WAIT_UNTIL_START(calls-- == 0);

		state1 = 1;
		NPT_YIELD();

		state1 = 2;

		NPT_YIELD();

		NPT_SUCCESS_IF(success1);

		state1 = 4;

		NPT_END_ON_ABORT(state1 = -1; calls = 3);
	}


public:
	int8_t state1;
	bool success1;
};

void
NestedProtothreadTest::testAbort()
{
	TestingAbortingThread thread;
	TestingAbortingThread0 thread0;

	// sanity checks
	TEST_ASSERT_FALSE(thread.isTaskRunning());
	TEST_ASSERT_EQUALS(thread.state1, 0);
	TEST_ASSERT_EQUALS(thread.state2, 0);
	TEST_ASSERT_EQUALS(thread.state3, 0);
	TEST_ASSERT_EQUALS(thread.getTaskDepth(), -1);
	// thread 0
	TEST_ASSERT_FALSE(thread0.isTaskRunning());
	TEST_ASSERT_EQUALS(thread0.state1, 0);
	TEST_ASSERT_EQUALS(thread0.getTaskDepth(), -1);

	// ZERO LEVEL ABORT
	// a non-running task may be aborted, which will only execute the abort handler
	TEST_ASSERT_EQUALS(thread.task1(xpcc::pt::ContextAbort), xpcc::pt::Abort);
	TEST_ASSERT_EQUALS(thread.state1, -1);
	// same for thread0
	TEST_ASSERT_EQUALS(thread0.task1(xpcc::pt::ContextAbort), xpcc::pt::Abort);
	TEST_ASSERT_EQUALS(thread0.state1, -1);

	// FIRST LEVEL ABORT
	// reset states
	thread.state1 = 0;
	thread0.state1 = 0;
	// wait 2 cycles out of 3
	for (uint8_t ii = 0; ii < 2; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Starting);
		TEST_ASSERT_EQUALS(thread.state1, 0);
		// thread0
		TEST_ASSERT_EQUALS(thread0.task1(this), xpcc::pt::Starting);
		TEST_ASSERT_EQUALS(thread0.state1, 0);
	}
	// now abort the task, which should execute the abort code
	TEST_ASSERT_EQUALS(thread.task1(xpcc::pt::ContextAbort), xpcc::pt::Abort);
	TEST_ASSERT_EQUALS(thread.state1, -1);
	// same for thread0
	TEST_ASSERT_EQUALS(thread0.task1(xpcc::pt::ContextAbort), xpcc::pt::Abort);
	TEST_ASSERT_EQUALS(thread0.state1, -1);

	// SECOND LEVEL ABORT
	// reset states
	thread.state1 = 0;
	// wait all cycles out of 3
	for (uint8_t ii = 0; ii < 3; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Starting);
		TEST_ASSERT_EQUALS(thread.state1, 0);
	}
	// task should progress to first yield
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);

	// task1 should spawn task2 now, which will "start" for one cycle
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 0);

	// task2 should run to first yield
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 1);

	// lets abort both tasks, both abort handlers should be executed immediately
	TEST_ASSERT_EQUALS(thread.task1(xpcc::pt::ContextAbort), xpcc::pt::Abort);
	TEST_ASSERT_EQUALS(thread.state1, -1);
	TEST_ASSERT_EQUALS(thread.state2, -1);

	// THIRD LEVEL ABORT
	// reset states
	thread.state1 = 0;
	thread.state2 = 0;
	for (uint8_t ii = 0; ii < 3; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Starting);
		TEST_ASSERT_EQUALS(thread.state1, 0);
	}
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 0);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 1);

	// task2 should spawn task3 now, which will "start" for one cycle
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 0);

	// task3 should run to first yield
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 1);

	// lets abort all three tasks
	TEST_ASSERT_EQUALS(thread.task1(xpcc::pt::ContextAbort), xpcc::pt::Abort);
	TEST_ASSERT_EQUALS(thread.state1, -1);
	TEST_ASSERT_EQUALS(thread.state2, -1);
	TEST_ASSERT_EQUALS(thread.state3, -1);

	// THIRD LEVEL INTERNAL ABORT
	// reset states
	thread.state1 = 0;
	thread.state2 = 0;
	thread.state3 = 0;
	for (uint8_t ii = 0; ii < 3; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Starting);
		TEST_ASSERT_EQUALS(thread.state1, 0);
	}
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 0);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 1);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 0);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 1);

	// task3 will not succeed and progress to next yield
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 2);

	// the task3 will now abort itself.
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Abort);
	TEST_ASSERT_EQUALS(thread.state1, -1);
	TEST_ASSERT_EQUALS(thread.state2, -1);
	TEST_ASSERT_EQUALS(thread.state3, -1);

	// THIRD LEVEL NO ABORT
	// reset states
	thread.state1 = 0;
	thread.state2 = 0;
	thread.state3 = 0;
	// task3 will now succeed
	thread.success3 = true;
	for (uint8_t ii = 0; ii < 3; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Starting);
		TEST_ASSERT_EQUALS(thread.state1, 0);
	}
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 0);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 1);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 0);
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 1);

	// the task3 should not abort itself anymore, but return successfully.
	// task2 will continue to next yield
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 3);
	TEST_ASSERT_EQUALS(thread.state3, 1);

	// task2 will return successfully
	// task1 will continue to next yield
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Running);
	TEST_ASSERT_EQUALS(thread.state1, 3);
	TEST_ASSERT_EQUALS(thread.state2, 3);

	// task1 will return successfully
	TEST_ASSERT_EQUALS(thread.task1(this), xpcc::pt::Success);
	TEST_ASSERT_EQUALS(thread.state1, 3);
}

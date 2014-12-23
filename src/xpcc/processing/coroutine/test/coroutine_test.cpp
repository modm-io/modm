// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/processing/coroutine.hpp>
#include "coroutine_test.hpp"

// ----------------------------------------------------------------------------
class TestingEmptyThread0 : public xpcc::co::Coroutine
{
public:
	TestingEmptyThread0()
	:	state(0), depth(0)
	{
	}

	xpcc::co::Result<bool>
	task1(void *ctx)
	{
		CO_BEGIN(ctx);

		state = 1;
		depth = getCoroutineDepth();

		CO_YIELD();

		state = 2;

		CO_END_RETURN(false);
	}

	xpcc::co::Result<bool>
	task2(void *ctx)
	{
		CO_BEGIN(ctx);

		state = 3;

		CO_END_RETURN(true);
	}

	uint8_t state;
	int8_t depth;
};

class TestingEmptyThread1 : public xpcc::co::NestedCoroutine<1>
{
public:
	TestingEmptyThread1()
	:	state(0), depth(0)
	{
	}

	xpcc::co::Result<bool>
	task1(void *ctx)
	{
		CO_BEGIN(ctx);

		state = 1;
		depth = getCoroutineDepth();

		CO_YIELD();

		state = 2;

		CO_END();
	}

	xpcc::co::Result<bool>
	task2(void *ctx)
	{
		CO_BEGIN(ctx);

		state = 3;

		CO_END();
	}

	uint8_t state;
	int8_t depth;
};

class TestingEmptyThread2 : public xpcc::co::NestedCoroutine<2>
{
public:
	TestingEmptyThread2()
	:	state(0), depth(0)
	{
	}

	xpcc::co::Result<bool>
	task1(void *ctx)
	{
		CO_BEGIN(ctx);

		state = 1;
		depth = getCoroutineDepth();

		CO_YIELD();

		state = 2;

		CO_END();
	}

	xpcc::co::Result<bool>
	task2(void *ctx)
	{
		CO_BEGIN(ctx);

		state = 3;

		CO_END();
	}

	uint8_t state;
	int8_t depth;
};

void
CoroutineTest::testClassMethods()
{
	uint8_t ctx_helper;
	void *ctx1 = this;
	void *ctx2 = &ctx_helper;

	// Nesting Depth of zero is specialized
	TestingEmptyThread0 thread0;
	// outside the nesting depth should be -1
	TEST_ASSERT_EQUALS(thread0.getCoroutineDepth(), -1);
	// nothing should have been modified
	TEST_ASSERT_EQUALS(thread0.state, 0);
	TEST_ASSERT_EQUALS(thread0.depth, 0);
	// still not running
	TEST_ASSERT_FALSE(thread0.isCoroutineRunning());

	// lets start a task, which will yield
	TEST_ASSERT_EQUALS(thread0.task1(ctx1).state, xpcc::co::Running);
	// now it should be running
	TEST_ASSERT_TRUE(thread0.isCoroutineRunning());
	// state should be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// depth should be 0
	TEST_ASSERT_EQUALS(thread0.depth, 0);

	// two tasks cannot run in the same context
	TEST_ASSERT_EQUALS(thread0.task2(ctx1).state, xpcc::co::WrongState);
	// but starting a task of the same class in another context
	// returns WrongContext, which can make parent class wait
	TEST_ASSERT_EQUALS(thread0.task2(ctx2).state, xpcc::co::WrongContext);
	// Similarly calling a running task1 in another context
	// also returns WrongContext
	TEST_ASSERT_EQUALS(thread0.task1(ctx2).state, xpcc::co::WrongContext);

	// state should still be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// but it should continue execution in the right context
	auto result1 = thread0.task1(ctx1);
	TEST_ASSERT_EQUALS(result1.state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result1.result, false);
	// state should be 2
	TEST_ASSERT_EQUALS(thread0.state, 2);
	// nothing is running anymore
	TEST_ASSERT_FALSE(thread0.isCoroutineRunning());

	// try the same with task2, which will end immediately
	auto result2 = thread0.task2(ctx1);
	TEST_ASSERT_EQUALS(result2.state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result2.result, true);
	// state should be 3
	TEST_ASSERT_EQUALS(thread0.state, 3);
	// not running anymore
	TEST_ASSERT_FALSE(thread0.isCoroutineRunning());

	TEST_ASSERT_EQUALS(thread0.getCoroutineDepth(), -1);
	// lets start a task, which will yield
	TEST_ASSERT_EQUALS(thread0.task1(ctx1).state, xpcc::co::Running);
	// state should be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// stop coroutine of thread0
	thread0.stopCoroutine();
	// not running anymore
	TEST_ASSERT_FALSE(thread0.isCoroutineRunning());


	// generic implementation with 1 nesting levels
	TestingEmptyThread1 thread1;
	TEST_ASSERT_EQUALS(thread1.getCoroutineDepth(), -1);
	TEST_ASSERT_EQUALS(thread1.state, 0);
	TEST_ASSERT_EQUALS(thread1.depth, 0);
	TEST_ASSERT_FALSE(thread1.isCoroutineRunning());

	TEST_ASSERT_EQUALS(thread1.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_TRUE(thread1.isCoroutineRunning());
	TEST_ASSERT_EQUALS(thread1.state, 1);
	TEST_ASSERT_EQUALS(thread1.depth, 0);

	TEST_ASSERT_EQUALS(thread1.task2(ctx1).state, xpcc::co::WrongState);
	TEST_ASSERT_EQUALS(thread1.task2(ctx2).state, xpcc::co::WrongContext);
	TEST_ASSERT_EQUALS(thread1.task1(ctx2).state, xpcc::co::WrongContext);

	TEST_ASSERT_EQUALS(thread1.state, 1);
	TEST_ASSERT_EQUALS(thread1.task1(ctx1).state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread1.state, 2);
	TEST_ASSERT_FALSE(thread1.isCoroutineRunning());

	TEST_ASSERT_EQUALS(thread1.task2(ctx1).state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread1.state, 3);
	TEST_ASSERT_FALSE(thread1.isCoroutineRunning());

	TEST_ASSERT_EQUALS(thread1.getCoroutineDepth(), -1);
	TEST_ASSERT_EQUALS(thread1.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread1.state, 1);
	thread1.stopCoroutine();
	TEST_ASSERT_FALSE(thread1.isCoroutineRunning());


	// generic implementation with 2 nesting levels
	TestingEmptyThread2 thread2;
	TEST_ASSERT_EQUALS(thread2.getCoroutineDepth(), -1);
	TEST_ASSERT_EQUALS(thread2.state, 0);
	TEST_ASSERT_EQUALS(thread2.depth, 0);
	TEST_ASSERT_FALSE(thread2.isCoroutineRunning());

	TEST_ASSERT_EQUALS(thread2.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_TRUE(thread2.isCoroutineRunning());
	TEST_ASSERT_EQUALS(thread2.state, 1);
	TEST_ASSERT_EQUALS(thread2.depth, 0);

	TEST_ASSERT_EQUALS(thread2.task2(ctx1).state, xpcc::co::WrongState);
	TEST_ASSERT_EQUALS(thread2.task2(ctx2).state, xpcc::co::WrongContext);
	TEST_ASSERT_EQUALS(thread2.task1(ctx2).state, xpcc::co::WrongContext);

	TEST_ASSERT_EQUALS(thread2.state, 1);
	TEST_ASSERT_EQUALS(thread2.task1(ctx1).state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread2.state, 2);
	TEST_ASSERT_FALSE(thread2.isCoroutineRunning());

	TEST_ASSERT_EQUALS(thread2.task2(ctx1).state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread2.state, 3);
	TEST_ASSERT_FALSE(thread2.isCoroutineRunning());

	TEST_ASSERT_EQUALS(thread2.getCoroutineDepth(), -1);
	TEST_ASSERT_EQUALS(thread2.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread2.state, 1);
	thread2.stopCoroutine();
	TEST_ASSERT_FALSE(thread2.isCoroutineRunning());
}


// ----------------------------------------------------------------------------
class TestingNestedThread : public xpcc::co::NestedCoroutine<2>
{
public:
	TestingNestedThread()
	:	depth1(0), depth2(0), depth3(0),
		state1(0), state2(0), state3(0),
		condition1(false), condition2(false), condition3(false),
		callResult1{0,false}, callResult2{0,false}, callResult3{0,false}
	{
	}

	xpcc::co::Result<bool>
	task1(void *ctx)
	{
		CO_BEGIN(ctx);

		state1 = 1;

		CO_WAIT_UNTIL(condition1);

		state1 = 2;
		depth1 = getCoroutineDepth();

		CO_YIELD();

		// manual spawn
		CO_WAIT_WHILE((callResult1 = task2(ctx)).state > xpcc::co::NestingError);

		state1 = 3;

		CO_YIELD();

		state1 = 4;

		CO_END();
	}

protected:
	xpcc::co::Result<bool>
	task2(void *ctx)
	{
		CO_BEGIN(ctx);

		state2 = 1;

		CO_WAIT_UNTIL(condition2);

		state2 = 2;
		depth2 = getCoroutineDepth();

		CO_YIELD();

		CO_WAIT_WHILE((callResult2 = task3(ctx)).state > xpcc::co::NestingError);

		state2 = 3;

		CO_YIELD();

		state2 = 4;

		CO_END();
	}

	xpcc::co::Result<bool>
	task3(void *ctx)
	{
		CO_BEGIN(ctx);

		state3 = 1;

		CO_WAIT_UNTIL(condition3);

		state3 = 2;
		depth3 = getCoroutineDepth();
		// this must return NestingError, since there is no more space
		// to buffer the next nested local continuation anymore!
		callResult3 = task3(ctx);

		CO_YIELD();

		state3 = 3;

		CO_END();
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
	xpcc::co::Result<bool> callResult1;
	xpcc::co::Result<bool> callResult2;
	xpcc::co::Result<bool> callResult3;
};
//*/

void
CoroutineTest::testNesting()
{
	uint8_t ctx_helper;
	void *ctx1 = this;
	void *ctx2 = &ctx_helper;

	TestingNestedThread thread;
	// sanity checks
	TEST_ASSERT_FALSE(thread.isCoroutineRunning());
	TEST_ASSERT_EQUALS(thread.state1, 0);
	TEST_ASSERT_EQUALS(thread.getCoroutineDepth(), -1);

	// should wait until the first condition
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);
	// task should wait here
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);

	// it should be running
	TEST_ASSERT_TRUE(thread.isCoroutineRunning());
	// we should be able to stop this task though
	thread.stopCoroutine();
	TEST_ASSERT_FALSE(thread.isCoroutineRunning());
	// and restart it
	thread.state1 = 0;
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);

	// lets release start condition 1
	thread.condition1 = true;
	// task should continue and yield
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	// check the state and depth
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.depth1, 0);

	// first manual spawn
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	// the callResult1 should be Starting
	TEST_ASSERT_EQUALS(thread.callResult1.state, xpcc::co::Running);
	// after another run, callResult1 should still be Starting
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult1.state, xpcc::co::Running);

	// task1 should reject any other contexts, even when currently spawning
	TEST_ASSERT_EQUALS(thread.task1(ctx2).state, xpcc::co::WrongContext);

	// lets release start condition 2
	thread.condition2 = true;
	// task2 will progress to first yield
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	// callResult1 should be Running
	TEST_ASSERT_EQUALS(thread.callResult1.state, xpcc::co::Running);
	// check the state and depth
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.depth2, 1);

	// task2 will progress to spawning task3
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	// callResult1 should be Running
	TEST_ASSERT_EQUALS(thread.callResult1.state, xpcc::co::Running);
	// callResult2 should be Starting
	TEST_ASSERT_EQUALS(thread.callResult2.state, xpcc::co::Running);

	// after another run, this should not change
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult1.state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult2.state, xpcc::co::Running);

	// lets release start condition 3
	thread.condition3 = true;
	// task3 will progress to first yield
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult1.state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult2.state, xpcc::co::Running);
	// check the states and depths
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 2);
	TEST_ASSERT_EQUALS(thread.depth3, 2);
	// we have exhausted the nesting capabilities
	TEST_ASSERT_EQUALS(thread.callResult3.state, xpcc::co::NestingError);

	// now we will begin to strip down the nestings
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult1.state, xpcc::co::Running);
	// task3 will complete
	TEST_ASSERT_EQUALS(thread.state3, 3);
	// callResult2 will return Stop
	TEST_ASSERT_EQUALS(thread.callResult2.state, xpcc::co::Stop);
	// task2 will continue until next yield
	TEST_ASSERT_EQUALS(thread.state2, 3);

	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Running);
	// task2 will complete
	TEST_ASSERT_EQUALS(thread.state2, 4);
	// callResult1 will return Stop
	TEST_ASSERT_EQUALS(thread.callResult1.state, xpcc::co::Stop);
	// task1 will continure until next yield
	TEST_ASSERT_EQUALS(thread.state1, 3);

	// task1 should end now
	TEST_ASSERT_EQUALS(thread.task1(ctx1).state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread.state1, 4);

	// nothing is running
	TEST_ASSERT_FALSE(thread.isCoroutineRunning());
}

uint8_t waits = 3;

class TestingSpawningThread : public xpcc::co::NestedCoroutine<1>
{
public:
	TestingSpawningThread()
	:	state(0)
	{
	}

	xpcc::co::Result<bool>
	parentCoroutine(void *ctx)
	{
		CO_BEGIN(ctx);

		state = 1;
		CO_YIELD();

		success = CO_CALL(spawningCoroutine(ctx, waits));

		state = 3;
		CO_YIELD();

		if (success)
			state = 4;
		else
			state = 5;

		CO_END();
	}

protected:
	xpcc::co::Result<bool>
	spawningCoroutine(void *ctx, uint8_t calls)
	{
		CO_BEGIN(ctx);

		CO_WAIT_UNTIL(startSpawningCoroutine(calls));

		state = 2;
		CO_YIELD();

		CO_WAIT_WHILE(runSpawningCoroutine(calls));

		if(isSpawningCoroutineSuccessful(calls))
			CO_RETURN(true);

		CO_END();
	}

	bool
	startSpawningCoroutine(uint8_t calls)
	{
		// manually implemented "protothread" without any side-effects on the NPT
		static uint8_t st_calls(0);

		if (st_calls++ < calls)
			return false;

		st_calls = 0;
		return true;
	}

	bool
	runSpawningCoroutine(uint8_t calls)
	{
		// manually implemented "protothread" without any side-effects on the NPT
		static uint8_t st_calls(0);

		if (st_calls++ < calls)
			return true;

		st_calls = 0;
		return false;
	}

	bool
	isSpawningCoroutineSuccessful(uint8_t calls)
	{ return (calls == 2); }

public:
	uint8_t state;
	bool success;
};

void
CoroutineTest::testSpawn()
{
	TestingSpawningThread thread;

	// sanity checks
	TEST_ASSERT_FALSE(thread.isCoroutineRunning());
	TEST_ASSERT_EQUALS(thread.state, 0);
	TEST_ASSERT_EQUALS(thread.getCoroutineDepth(), -1);

	// should wait until the first condition
	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 1);
	// task should require `waits` number of calls
	for (uint8_t ii = 0; ii < waits; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
		TEST_ASSERT_EQUALS(thread.state, 1);
	}
	// now spawning task has started
	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 2);
	// task should require `waits` number of calls again
	for (uint8_t ii = 0; ii < waits; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
		TEST_ASSERT_EQUALS(thread.state, 2);
	}
	// now spawning task has finished
	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 3);
	TEST_ASSERT_EQUALS(thread.success, (waits == 2) ? true : false);

	// now parent task has finished
	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread.state, (waits == 2) ? 4 : 5);
}

#include <xpcc/math/filter/moving_average.hpp>

class TestingSpawningComplexThread : public xpcc::co::NestedCoroutine<1>
{
public:
	TestingSpawningComplexThread()
	:	state(0),
		result1(0), result2(0),
		resultLocal1(0), resultLocal2(0),
		resultIf1(0), resultIf2(0),
		resultFunction1(0), resultFunction2(0),
		resultStack1(0), resultStack2(0)
	{
	}

	xpcc::co::Result<uint16_t>
	parentCoroutine(void *ctx)
	{
		// this is an unelegant way of using 'local' variables in a Coroutine.
		uint8_t rslt1;
		int8_t rslt2;

		CO_BEGIN(ctx);

		state = 1;
		CO_YIELD();

		result1 = CO_CALL(spawningCoroutine1(ctx));
		result2 = CO_CALL(spawningCoroutine2(ctx));

		state = 2;
		CO_YIELD();

		rslt1 = CO_CALL(spawningCoroutine1(ctx));
		resultLocal1 = rslt1;

		rslt2 = CO_CALL(spawningCoroutine2(ctx));
		resultLocal2 = rslt2;

		state = 3;
		CO_YIELD();

		if (CO_CALL(spawningCoroutine1(ctx)) == 42)
		{
			resultIf1 = 42;
		}
		if (CO_CALL(spawningCoroutine2(ctx)) == 42)
		{
			resultIf2 = 42;
		}

		state = 4;
		CO_YIELD();

		setResultFunction1(CO_CALL(spawningCoroutine1(ctx)));
		setResultFunction2(CO_CALL(spawningCoroutine2(ctx)));

		state = 5;
		CO_YIELD();

		// I would expect this to work, especially after the previous examples,
		// but this 'crosses initialization of rslt'
//		{
//			uint8_t rslt = CO_CALL(spawningCoroutine1(ctx));
//			resultStack1 = rslt;
//		}
//		{
//			int8_t rslt = CO_CALL(spawningCoroutine2(ctx));
//			resultStack2 = rslt;
//		}
//
//		state = 6;
//		CO_YIELD();

		CO_END_RETURN(static_cast<uint16_t>(result1 + resultLocal1));
	}

protected:
	xpcc::co::Result<uint8_t>
	spawningCoroutine1(void *ctx)
	{
		CO_BEGIN(ctx);

		CO_RETURN(42);

		CO_END();
	}

	xpcc::co::Result<int8_t>
	spawningCoroutine2(void *ctx)
	{
		CO_BEGIN(ctx);

		if (waits >= 2)
		{
			CO_RETURN(42);
		}

		CO_END_RETURN(-42);
	}

	void
	setResultFunction1(uint8_t value)
	{
		resultFunction1 = value;
	}

	void
	setResultFunction2(const int8_t &value)
	{
		resultFunction2 = value;
	}

public:
	uint8_t state;
	uint8_t result1;
	int8_t result2;
	uint8_t resultLocal1;
	int8_t resultLocal2;
	uint8_t resultIf1;
	int8_t resultIf2;
	uint8_t resultFunction1;
	int8_t resultFunction2;
	uint8_t resultStack1;
	int8_t resultStack2;
};


void
CoroutineTest::testComplexSpawn()
{
	TestingSpawningComplexThread thread;

	// sanity checks
	TEST_ASSERT_FALSE(thread.isCoroutineRunning());
	TEST_ASSERT_EQUALS(thread.state, 0);
	TEST_ASSERT_EQUALS(thread.getCoroutineDepth(), -1);

	// should wait until the first condition
	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 1);

	waits = 1;
	// now run all CO_CALLs until yield
	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 2);

	TEST_ASSERT_EQUALS(thread.result1, 42);
	TEST_ASSERT_EQUALS(thread.result2, -42);

	waits = 3;
	// run second, local CO_CALLs
	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 3);

	TEST_ASSERT_EQUALS(thread.resultLocal1, 42);
	TEST_ASSERT_EQUALS(thread.resultLocal2, 42);

	waits = 1;
	// run third, if CO_CALLs
	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 4);

	TEST_ASSERT_EQUALS(thread.resultIf1, 42);
	TEST_ASSERT_EQUALS(thread.resultIf2, 0);

	waits = 3;
	// run third, if CO_CALLs
	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 5);

	TEST_ASSERT_EQUALS(thread.resultFunction1, 42);
	TEST_ASSERT_EQUALS(thread.resultFunction2, 42);

//	waits = 3;
//	// run fourth, stack CO_CALLs
//	TEST_ASSERT_EQUALS(thread.parentCoroutine(this).state, xpcc::co::Running);
//	TEST_ASSERT_EQUALS(thread.state, 5);
//
//	TEST_ASSERT_EQUALS(thread.resultStack1, 42);
//	TEST_ASSERT_EQUALS(thread.resultStack2, 42);

	auto result = thread.parentCoroutine(this);
	TEST_ASSERT_EQUALS(result.state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result.result, 42+42);
}


class TestingCaseLabelThread : public xpcc::co::Coroutine
{
public:
	xpcc::co::Result<bool>
	coroutine(void *ctx)
	{
		CO_BEGIN(ctx);
		// 1 case label

		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		// 101 case labels

		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		// 201 case labels

		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();CO_YIELD();
		// 251 case labels

		CO_YIELD();CO_YIELD();CO_YIELD();
		// 254 case labels

		// uncommenting this case label must now trigger a static assert warning!
		//CO_YIELD();

		CO_END_RETURN(true);
	}
};

void
CoroutineTest::testCaseNumbers()
{
	TestingCaseLabelThread thread;

	// this routine must be called 253 times
	for(uint32_t ii=0; ii < 253; ii++)
	{
		TEST_ASSERT_EQUALS(thread.coroutine(this).state, xpcc::co::Running);
	}
	// the 254th time must return
	auto result = thread.coroutine(this);
	TEST_ASSERT_EQUALS(result.state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result.result, true);
}

class TestingCaseEnumClassThread : public xpcc::co::Coroutine
{
public:
	enum class
	Animal
	{
		Dog,
		Cat,
	};

	xpcc::co::Result<Animal>
	coroutine(void *ctx)
	{
		CO_BEGIN(ctx);

		CO_YIELD();		// generate at least one additional case statement

		CO_END_RETURN(Animal::Cat);
	}
};

void
CoroutineTest::testReturnEnumClass()
{
	TestingCaseEnumClassThread thread;

	// run once; coroutine will yield
	TEST_ASSERT_EQUALS(thread.coroutine(this).state, xpcc::co::Running);

	// now we should get a cat
	auto result = thread.coroutine(this);
	TEST_ASSERT_EQUALS(result.state, xpcc::co::Stop);
	TEST_ASSERT_TRUE(result.result == TestingCaseEnumClassThread::Animal::Cat);
}

class TestingCaseVoidClassThread : public xpcc::co::Coroutine
{
public:
	xpcc::co::Result<void>
	coroutine(void *ctx)
	{
		CO_BEGIN(ctx);

		CO_YIELD();		// generate at least one additional case statement

		CO_END();	// return nothing!
	}
};

void
CoroutineTest::testReturnVoidClass()
{
	TestingCaseVoidClassThread thread;

	// run once; coroutine will yield
	TEST_ASSERT_EQUALS(thread.coroutine(this).state, xpcc::co::Running);

	auto result = thread.coroutine(this);
	TEST_ASSERT_EQUALS(result.state, xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result.result, xpcc::co::Stop);
	TEST_ASSERT_TRUE(sizeof(result) == 1);

	// this now returns the state
	auto result2 = CO_CALL_BLOCKING(thread.coroutine(this));
	TEST_ASSERT_EQUALS(result2, xpcc::co::Stop);
}

// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/processing/coroutine.hpp>
#include "resumable_test.hpp"

// ----------------------------------------------------------------------------
class TestingEmptyThread0 : public xpcc::co::NestedResumable<1>
{
public:
	TestingEmptyThread0()
	:	state(0), depth(0)
	{
	}

	xpcc::co::Result<bool>
	task1()
	{
		CO_BEGIN();

		state = 1;
		depth = getResumableDepth();

		CO_YIELD();

		state = 2;

		CO_END_RETURN(false);
	}

	xpcc::co::Result<bool>
	task2()
	{
		CO_BEGIN();

		state = 3;

		CO_END_RETURN(true);
	}

	uint8_t state;
	int8_t depth;
};

class TestingEmptyThread1 : public xpcc::co::NestedResumable<2>
{
public:
	TestingEmptyThread1()
	:	state(0), depth(0)
	{
	}

	xpcc::co::Result<bool>
	task1()
	{
		CO_BEGIN();

		state = 1;
		depth = getResumableDepth();

		CO_YIELD();

		state = 2;

		CO_END();
	}

	xpcc::co::Result<bool>
	task2()
	{
		CO_BEGIN();

		state = 3;

		CO_END();
	}

	uint8_t state;
	int8_t depth;
};

class TestingEmptyThread2 : public xpcc::co::NestedResumable<3>
{
public:
	TestingEmptyThread2()
	:	state(0), depth(0)
	{
	}

	xpcc::co::Result<bool>
	task1()
	{
		CO_BEGIN();

		state = 1;
		depth = getResumableDepth();

		CO_YIELD();

		state = 2;

		CO_END();
	}

	xpcc::co::Result<bool>
	task2()
	{
		CO_BEGIN();

		state = 3;

		CO_END();
	}

	uint8_t state;
	int8_t depth;
};

void
ResumableTest::testClassMethods()
{
	// Nesting Depth of zero is specialized
	TestingEmptyThread0 thread0;
	// outside the nesting depth should be -1
	TEST_ASSERT_EQUALS(thread0.getResumableDepth(), -1);
	// nothing should have been modified
	TEST_ASSERT_EQUALS(thread0.state, 0);
	TEST_ASSERT_EQUALS(thread0.depth, 0);
	// still not running
	TEST_ASSERT_FALSE(thread0.isResumableRunning());

	// lets start a task, which will yield
	TEST_ASSERT_EQUALS(thread0.task1().getState(), xpcc::co::Running);
	// now it should be running
	TEST_ASSERT_TRUE(thread0.isResumableRunning());
	// state should be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// depth should be 0
	TEST_ASSERT_EQUALS(thread0.depth, 0);

	// two tasks cannot run in the same context
	TEST_ASSERT_EQUALS(thread0.task2().getState(), xpcc::co::WrongState);

	// state should still be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// but it should continue execution in the right context
	auto result1 = thread0.task1();
	TEST_ASSERT_EQUALS(result1.getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result1.getResult(), false);
	// state should be 2
	TEST_ASSERT_EQUALS(thread0.state, 2);
	// nothing is running anymore
	TEST_ASSERT_FALSE(thread0.isResumableRunning());

	// try the same with task2, which will end immediately
	auto result2 = thread0.task2();
	TEST_ASSERT_EQUALS(result2.getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result2.getResult(), true);
	// state should be 3
	TEST_ASSERT_EQUALS(thread0.state, 3);
	// not running anymore
	TEST_ASSERT_FALSE(thread0.isResumableRunning());

	TEST_ASSERT_EQUALS(thread0.getResumableDepth(), -1);
	// lets start a task, which will yield
	TEST_ASSERT_EQUALS(thread0.task1().getState(), xpcc::co::Running);
	// state should be 1
	TEST_ASSERT_EQUALS(thread0.state, 1);
	// stop coroutine of thread0
	thread0.stopResumable();
	// not running anymore
	TEST_ASSERT_FALSE(thread0.isResumableRunning());


	// generic implementation with 1 nesting levels
	TestingEmptyThread1 thread1;
	TEST_ASSERT_EQUALS(thread1.getResumableDepth(), -1);
	TEST_ASSERT_EQUALS(thread1.state, 0);
	TEST_ASSERT_EQUALS(thread1.depth, 0);
	TEST_ASSERT_FALSE(thread1.isResumableRunning());

	TEST_ASSERT_EQUALS(thread1.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_TRUE(thread1.isResumableRunning());
	TEST_ASSERT_EQUALS(thread1.state, 1);
	TEST_ASSERT_EQUALS(thread1.depth, 0);

	TEST_ASSERT_EQUALS(thread1.task2().getState(), xpcc::co::WrongState);

	TEST_ASSERT_EQUALS(thread1.state, 1);
	TEST_ASSERT_EQUALS(thread1.task1().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread1.state, 2);
	TEST_ASSERT_FALSE(thread1.isResumableRunning());

	TEST_ASSERT_EQUALS(thread1.task2().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread1.state, 3);
	TEST_ASSERT_FALSE(thread1.isResumableRunning());

	TEST_ASSERT_EQUALS(thread1.getResumableDepth(), -1);
	TEST_ASSERT_EQUALS(thread1.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread1.state, 1);
	thread1.stopResumable();
	TEST_ASSERT_FALSE(thread1.isResumableRunning());


	// generic implementation with 2 nesting levels
	TestingEmptyThread2 thread2;
	TEST_ASSERT_EQUALS(thread2.getResumableDepth(), -1);
	TEST_ASSERT_EQUALS(thread2.state, 0);
	TEST_ASSERT_EQUALS(thread2.depth, 0);
	TEST_ASSERT_FALSE(thread2.isResumableRunning());

	TEST_ASSERT_EQUALS(thread2.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_TRUE(thread2.isResumableRunning());
	TEST_ASSERT_EQUALS(thread2.state, 1);
	TEST_ASSERT_EQUALS(thread2.depth, 0);

	TEST_ASSERT_EQUALS(thread2.task2().getState(), xpcc::co::WrongState);

	TEST_ASSERT_EQUALS(thread2.state, 1);
	TEST_ASSERT_EQUALS(thread2.task1().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread2.state, 2);
	TEST_ASSERT_FALSE(thread2.isResumableRunning());

	TEST_ASSERT_EQUALS(thread2.task2().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread2.state, 3);
	TEST_ASSERT_FALSE(thread2.isResumableRunning());

	TEST_ASSERT_EQUALS(thread2.getResumableDepth(), -1);
	TEST_ASSERT_EQUALS(thread2.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread2.state, 1);
	thread2.stopResumable();
	TEST_ASSERT_FALSE(thread2.isResumableRunning());
}


// ----------------------------------------------------------------------------
class TestingNestedThread : public xpcc::co::NestedResumable<3>
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
	task1()
	{
		CO_BEGIN();

		state1 = 1;

		CO_WAIT_UNTIL(condition1);

		state1 = 2;
		depth1 = getResumableDepth();

		CO_YIELD();

		// manual spawn
		CO_WAIT_WHILE((callResult1 = task2()).getState() > xpcc::co::NestingError);

		state1 = 3;

		CO_YIELD();

		state1 = 4;

		CO_END();
	}

protected:
	xpcc::co::Result<bool>
	task2()
	{
		CO_BEGIN();

		state2 = 1;

		CO_WAIT_UNTIL(condition2);

		state2 = 2;
		depth2 = getResumableDepth();

		CO_YIELD();

		CO_WAIT_WHILE((callResult2 = task3()).getState() > xpcc::co::NestingError);

		state2 = 3;

		CO_YIELD();

		state2 = 4;

		CO_END();
	}

	xpcc::co::Result<bool>
	task3()
	{
		CO_BEGIN();

		state3 = 1;

		CO_WAIT_UNTIL(condition3);

		state3 = 2;
		depth3 = getResumableDepth();
		// this must return NestingError, since there is no more space
		// to buffer the next nested local continuation anymore!
		callResult3 = task3();

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
ResumableTest::testNesting()
{
	TestingNestedThread thread;
	// sanity checks
	TEST_ASSERT_FALSE(thread.isResumableRunning());
	TEST_ASSERT_EQUALS(thread.state1, 0);
	TEST_ASSERT_EQUALS(thread.getResumableDepth(), -1);

	// should wait until the first condition
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);
	// task should wait here
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);

	// it should be running
	TEST_ASSERT_TRUE(thread.isResumableRunning());
	// we should be able to stop this task though
	thread.stopResumable();
	TEST_ASSERT_FALSE(thread.isResumableRunning());
	// and restart it
	thread.state1 = 0;
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);

	// lets release start condition 1
	thread.condition1 = true;
	// task should continue and yield
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	// check the state and depth
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.depth1, 0);

	// first manual spawn
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	// the callResult1 should be Starting
	TEST_ASSERT_EQUALS(thread.callResult1.getState(), xpcc::co::Running);
	// after another run, callResult1 should still be Starting
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult1.getState(), xpcc::co::Running);

	// lets release start condition 2
	thread.condition2 = true;
	// task2 will progress to first yield
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	// callResult1 should be Running
	TEST_ASSERT_EQUALS(thread.callResult1.getState(), xpcc::co::Running);
	// check the state and depth
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.depth2, 1);

	// task2 will progress to spawning task3
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	// callResult1 should be Running
	TEST_ASSERT_EQUALS(thread.callResult1.getState(), xpcc::co::Running);
	// callResult2 should be Starting
	TEST_ASSERT_EQUALS(thread.callResult2.getState(), xpcc::co::Running);

	// after another run, this should not change
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult1.getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult2.getState(), xpcc::co::Running);

	// lets release start condition 3
	thread.condition3 = true;
	// task3 will progress to first yield
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult1.getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult2.getState(), xpcc::co::Running);
	// check the states and depths
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state3, 2);
	TEST_ASSERT_EQUALS(thread.depth3, 2);
	// we have exhausted the nesting capabilities
	TEST_ASSERT_EQUALS(thread.callResult3.getState(), xpcc::co::NestingError);

	// now we will begin to strip down the nestings
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.callResult1.getState(), xpcc::co::Running);
	// task3 will complete
	TEST_ASSERT_EQUALS(thread.state3, 3);
	// callResult2 will return Stop
	TEST_ASSERT_EQUALS(thread.callResult2.getState(), xpcc::co::Stop);
	// task2 will continue until next yield
	TEST_ASSERT_EQUALS(thread.state2, 3);

	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Running);
	// task2 will complete
	TEST_ASSERT_EQUALS(thread.state2, 4);
	// callResult1 will return Stop
	TEST_ASSERT_EQUALS(thread.callResult1.getState(), xpcc::co::Stop);
	// task1 will continure until next yield
	TEST_ASSERT_EQUALS(thread.state1, 3);

	// task1 should end now
	TEST_ASSERT_EQUALS(thread.task1().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread.state1, 4);

	// nothing is running
	TEST_ASSERT_FALSE(thread.isResumableRunning());
}

uint8_t waits = 3;

class TestingSpawningThread : public xpcc::co::NestedResumable<2>
{
public:
	TestingSpawningThread()
	:	state(0)
	{
	}

	xpcc::co::Result<bool>
	parentResumable()
	{
		CO_BEGIN();

		state = 1;
		CO_YIELD();

		success = CO_CALL(spawningResumable(waits));

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
	spawningResumable(uint8_t calls)
	{
		CO_BEGIN();

		CO_WAIT_UNTIL(startSpawningResumable(calls));

		state = 2;
		CO_YIELD();

		CO_WAIT_WHILE(runSpawningResumable(calls));

		if(isSpawningResumableSuccessful(calls))
			CO_RETURN(true);

		CO_END();
	}

	bool
	startSpawningResumable(uint8_t calls)
	{
		// manually implemented "protothread" without any side-effects on the NPT
		static uint8_t st_calls(0);

		if (st_calls++ < calls)
			return false;

		st_calls = 0;
		return true;
	}

	bool
	runSpawningResumable(uint8_t calls)
	{
		// manually implemented "protothread" without any side-effects on the NPT
		static uint8_t st_calls(0);

		if (st_calls++ < calls)
			return true;

		st_calls = 0;
		return false;
	}

	bool
	isSpawningResumableSuccessful(uint8_t calls)
	{ return (calls == 2); }

public:
	uint8_t state;
	bool success;
};

void
ResumableTest::testSpawn()
{
	TestingSpawningThread thread;

	// sanity checks
	TEST_ASSERT_FALSE(thread.isResumableRunning());
	TEST_ASSERT_EQUALS(thread.state, 0);
	TEST_ASSERT_EQUALS(thread.getResumableDepth(), -1);

	// should wait until the first condition
	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 1);
	// task should require `waits` number of calls
	for (uint8_t ii = 0; ii < waits; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
		TEST_ASSERT_EQUALS(thread.state, 1);
	}
	// now spawning task has started
	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 2);
	// task should require `waits` number of calls again
	for (uint8_t ii = 0; ii < waits; ++ii)
	{
		TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
		TEST_ASSERT_EQUALS(thread.state, 2);
	}
	// now spawning task has finished
	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 3);
	TEST_ASSERT_EQUALS(thread.success, (waits == 3) ? true : false);

	// now parent task has finished
	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread.state, (waits == 3) ? 4 : 5);
}

#include <xpcc/math/filter/moving_average.hpp>

class TestingSpawningComplexThread : public xpcc::co::NestedResumable<2>
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
	parentResumable()
	{
		// this is an unelegant way of using 'local' variables in a Resumable.
		uint8_t rslt1;
		int8_t rslt2;

		CO_BEGIN();

		state = 1;
		CO_YIELD();

		result1 = CO_CALL(spawningResumable1());
		result2 = CO_CALL(spawningResumable2());

		state = 2;
		CO_YIELD();

		rslt1 = CO_CALL(spawningResumable1());
		resultLocal1 = rslt1;

		rslt2 = CO_CALL(spawningResumable2());
		resultLocal2 = rslt2;

		state = 3;
		CO_YIELD();

		if (CO_CALL(spawningResumable1()) == 42)
		{
			resultIf1 = 42;
		}
		if (CO_CALL(spawningResumable2()) == 42)
		{
			resultIf2 = 42;
		}

		state = 4;
		CO_YIELD();

		setResultFunction1(CO_CALL(spawningResumable1()));
		setResultFunction2(CO_CALL(spawningResumable2()));

		state = 5;
		CO_YIELD();

		// I would expect this to work, especially after the previous examples,
		// but this 'crosses initialization of rslt'
//		{
//			uint8_t rslt = CO_CALL(spawningResumable1());
//			resultStack1 = rslt;
//		}
//		{
//			int8_t rslt = CO_CALL(spawningResumable2());
//			resultStack2 = rslt;
//		}
//
//		state = 6;
//		CO_YIELD();

		CO_END_RETURN(static_cast<uint16_t>(result1 + resultLocal1));
	}

protected:
	xpcc::co::Result<uint8_t>
	spawningResumable1()
	{
		CO_BEGIN();

		CO_RETURN(42);

		CO_END();
	}

	xpcc::co::Result<int8_t>
	spawningResumable2()
	{
		CO_BEGIN();

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
ResumableTest::testComplexSpawn()
{
	TestingSpawningComplexThread thread;

	// sanity checks
	TEST_ASSERT_FALSE(thread.isResumableRunning());
	TEST_ASSERT_EQUALS(thread.state, 0);
	TEST_ASSERT_EQUALS(thread.getResumableDepth(), -1);

	// should wait until the first condition
	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 1);

	waits = 1;
	// now run all CO_CALLs until yield
	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 2);

	TEST_ASSERT_EQUALS(thread.result1, 42);
	TEST_ASSERT_EQUALS(thread.result2, -42);

	waits = 3;
	// run second, local CO_CALLs
	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 3);

	TEST_ASSERT_EQUALS(thread.resultLocal1, 42);
	TEST_ASSERT_EQUALS(thread.resultLocal2, 42);

	waits = 1;
	// run third, if CO_CALLs
	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 4);

	TEST_ASSERT_EQUALS(thread.resultIf1, 42);
	TEST_ASSERT_EQUALS(thread.resultIf2, 0);

	waits = 3;
	// run third, if CO_CALLs
	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state, 5);

	TEST_ASSERT_EQUALS(thread.resultFunction1, 42);
	TEST_ASSERT_EQUALS(thread.resultFunction2, 42);

//	waits = 3;
//	// run fourth, stack CO_CALLs
//	TEST_ASSERT_EQUALS(thread.parentResumable().getState(), xpcc::co::Running);
//	TEST_ASSERT_EQUALS(thread.state, 5);
//
//	TEST_ASSERT_EQUALS(thread.resultStack1, 42);
//	TEST_ASSERT_EQUALS(thread.resultStack2, 42);

	auto result = thread.parentResumable();
	TEST_ASSERT_EQUALS(result.getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result.getResult(), 42+42);
}


class TestingCaseLabelThread : public xpcc::co::NestedResumable<>
{
public:
	xpcc::co::Result<bool>
	coroutine()
	{
		CO_BEGIN();
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
//		CO_YIELD();

		CO_END_RETURN(true);
	}
};

void
ResumableTest::testCaseNumbers()
{
	TestingCaseLabelThread thread;

	// this routine must be called 253 times
	for(uint32_t ii=0; ii < 253; ii++)
	{
		TEST_ASSERT_EQUALS(thread.coroutine().getState(), xpcc::co::Running);
	}
	// the 254th time must return
	auto result = thread.coroutine();
	TEST_ASSERT_EQUALS(result.getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result.getResult(), true);
}

class TestingCaseEnumClassThread : public xpcc::co::NestedResumable<>
{
public:
	enum class
	Animal
	{
		Dog,
		Cat,
	};

	xpcc::co::Result<Animal>
	coroutine()
	{
		CO_BEGIN();

		CO_YIELD();		// generate at least one additional case statement

		CO_END_RETURN(Animal::Cat);
	}
};

void
ResumableTest::testReturnEnumClass()
{
	TestingCaseEnumClassThread thread;

	// run once; coroutine will yield
	TEST_ASSERT_EQUALS(thread.coroutine().getState(), xpcc::co::Running);

	// now we should get a cat
	auto result = thread.coroutine();
	TEST_ASSERT_EQUALS(result.getState(), xpcc::co::Stop);
	TEST_ASSERT_TRUE(result.getResult() == TestingCaseEnumClassThread::Animal::Cat);
}

class TestingCaseVoidClassThread : public xpcc::co::NestedResumable<>
{
public:
	xpcc::co::Result<void>
	coroutine()
	{
		CO_BEGIN();

		CO_YIELD();		// generate at least one additional case statement

		CO_END();	// return nothing!
	}
};

void
ResumableTest::testReturnVoidClass()
{
	TestingCaseVoidClassThread thread;

	// run once; coroutine will yield
	TEST_ASSERT_EQUALS(thread.coroutine().getState(), xpcc::co::Running);

	auto result = thread.coroutine();
	TEST_ASSERT_EQUALS(result.getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(result.getResult(), xpcc::co::Stop);
	TEST_ASSERT_TRUE(sizeof(result) == 1);

	// this now returns the state
	auto result2 = CO_CALL_BLOCKING(thread.coroutine());
	TEST_ASSERT_EQUALS(result2, xpcc::co::Stop);
}

class TestingNonMutuallyExclusiveResumables : public xpcc::co::Resumable<3>
{
public:
	xpcc::co::Result<void>
	call0()
	{
		CO_BEGIN(0);

		state0 = 0;
		CO_YIELD();

		state0 = 1;
		CO_END();
	}

	xpcc::co::Result<void>
	call1()
	{
		CO_BEGIN(1);

		state1 = 0;
		CO_YIELD();

		state1 = 1;
		CO_YIELD();

		state1 = 2;
		CO_END();
	}

	xpcc::co::Result<void>
	call2()
	{
		CO_BEGIN(2);

		state2 = 0;
		CO_YIELD();

		state2 = 1;
		CO_YIELD();

		state2 = 2;
		CO_CALL(call1());

		state2 = 3;
		CO_CALL(call0());

		state2 = 4;
		CO_END();
	}

	uint8_t state0;
	uint8_t state1;
	uint8_t state2;
};

void
ResumableTest::testNonNestedResumables()
{
	TestingNonMutuallyExclusiveResumables thread;

	// nothing should be running
	TEST_ASSERT_FALSE(thread.areAnyResumablesRunning());
	TEST_ASSERT_FALSE(thread.isResumableRunning(0));
	TEST_ASSERT_FALSE(thread.isResumableRunning(1));
	TEST_ASSERT_FALSE(thread.isResumableRunning(2));
	TEST_ASSERT_FALSE(thread.isResumableRunning(100));

	// run once; coroutine will yield, but not finish
	TEST_ASSERT_EQUALS(thread.call0().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state0, 0);

	// coroutine 0 should be running
	TEST_ASSERT_TRUE(thread.areAnyResumablesRunning());
	TEST_ASSERT_TRUE(thread.isResumableRunning(0));
	TEST_ASSERT_FALSE(thread.isResumableRunning(1));
	TEST_ASSERT_FALSE(thread.isResumableRunning(2));
	TEST_ASSERT_FALSE(thread.isResumableRunning(100));

	TEST_ASSERT_FALSE(thread.areAllResumablesRunning({0, 1}));
	TEST_ASSERT_FALSE(thread.joinResumables({0, 1}));
	TEST_ASSERT_TRUE(thread.joinResumables({1, 2}));
	TEST_ASSERT_FALSE(thread.areAllResumablesRunning({1, 2}));
	TEST_ASSERT_TRUE(thread.areAnyResumablesRunning({0, 1}));
	TEST_ASSERT_FALSE(thread.areAnyResumablesRunning({1, 2}));
	TEST_ASSERT_TRUE(thread.areAnyResumablesRunning({0, 1, 100}));
	TEST_ASSERT_FALSE(thread.areAllResumablesRunning({0, 100}));

	// the other coroutines should be able to run at the same time
	TEST_ASSERT_EQUALS(thread.call1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state1, 0);
	TEST_ASSERT_EQUALS(thread.call2().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state2, 0);

	// second round of calls, call0 stops
	TEST_ASSERT_EQUALS(thread.call0().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread.state0, 1);
	// other cos keep running
	TEST_ASSERT_EQUALS(thread.call1().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state1, 1);
	TEST_ASSERT_EQUALS(thread.call2().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state2, 1);

	// call1 stops
	TEST_ASSERT_EQUALS(thread.call1().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread.state1, 2);

	// call2 spawns call1
	TEST_ASSERT_EQUALS(thread.call2().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state1, 0);

	// call0 should be able to run before call2 finishes spawning call1
	TEST_ASSERT_EQUALS(thread.call0().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state0, 0);

	// continue call2
	TEST_ASSERT_EQUALS(thread.call2().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state2, 2);
	TEST_ASSERT_EQUALS(thread.state1, 1);

	// call0 should be able to run before call2 finishes spawning call1
	TEST_ASSERT_EQUALS(thread.call0().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread.state0, 1);

	// continue call2, which starts call0
	TEST_ASSERT_EQUALS(thread.call2().getState(), xpcc::co::Running);
	TEST_ASSERT_EQUALS(thread.state2, 3);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state0, 0);

	// continue call2, call0 ends
	TEST_ASSERT_EQUALS(thread.call2().getState(), xpcc::co::Stop);
	TEST_ASSERT_EQUALS(thread.state2, 4);
	TEST_ASSERT_EQUALS(thread.state1, 2);
	TEST_ASSERT_EQUALS(thread.state0, 1);
}

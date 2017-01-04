/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/processing/protothread.hpp>

#include "protothread_test.hpp"

// ----------------------------------------------------------------------------
class TestingEmptyThread : public xpcc::pt::Protothread
{
public:
	bool
	run()
	{
		PT_BEGIN();
		PT_END();
	}
};

void
ProtothreadTest::testClassMethods()
{
	TestingEmptyThread thread;
	TEST_ASSERT_TRUE(thread.isRunning());
	
	TEST_ASSERT_FALSE(thread.run());
	TEST_ASSERT_FALSE(thread.isRunning());
	
	thread.restart();
	TEST_ASSERT_TRUE(thread.isRunning());
	
	thread.stop();
	TEST_ASSERT_FALSE(thread.isRunning());
	TEST_ASSERT_FALSE(thread.run());
}

// ----------------------------------------------------------------------------
class TestingChildThread : public xpcc::pt::Protothread
{
public:
	TestingChildThread() :
		state(0), condition(false)
	{
	}
	
	bool
	run()
	{
		PT_BEGIN();
		
		this->state = 1;
		
		PT_WAIT_UNTIL(this->condition);
		
		this->state = 2;
		
		PT_END();
	}
	
	uint8_t state;
	bool condition;
};

class TestingThread : public xpcc::pt::Protothread
{
public:
	TestingThread() :
		state(0), condition(false),
		restartCondition(true)
	{
	}
	
	bool
	run()
	{
		PT_BEGIN();
		
		this->state = 1;
		
		PT_YIELD();
		
		this->state = 2;
		
		PT_WAIT_UNTIL(this->condition);
		
		this->state = 3;
		
		PT_SPAWN(child);
		
		this->state = 4;
		
		PT_YIELD();
		
		child.restart();
		PT_WAIT_THREAD(child);
		
		this->state = 5;
		
		PT_YIELD();
		
		this->state = 6;
		
		PT_END();
	}
	
	uint8_t state;
	bool condition;
	bool restartCondition;
	
	TestingChildThread child;
};

void
ProtothreadTest::testMacros()
{
	TestingThread thread;
	
	TEST_ASSERT_EQUALS(thread.state, 0);
	
	// first yield
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 1);
	
	// now the it should wait until testCondition is set
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 2);
	
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 2);
	
	// child thread should be in its initial state
	TEST_ASSERT_EQUALS(thread.child.state, 0);
	
	// execution continues
	thread.condition = true;
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 3);
	
	// now the code has reached PT_SPAWN() which runs the child
	TEST_ASSERT_EQUALS(thread.child.state, 1);
	
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 3);
	TEST_ASSERT_EQUALS(thread.child.state, 1);
	
	thread.child.condition = true;
	
	// now the child can continue and will run into the exit
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 4);
	TEST_ASSERT_EQUALS(thread.child.state, 2);
	TEST_ASSERT_FALSE(thread.child.isRunning());
	
	thread.child.condition = false;
	
	// the child is restarted
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 4);
	TEST_ASSERT_EQUALS(thread.child.state, 1);
	
	thread.child.condition = true;
	
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 5);
	TEST_ASSERT_TRUE(thread.isRunning());
	
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 6);
	
	// thread reaches end, therefore execution should stop
	TEST_ASSERT_FALSE(thread.isRunning());
	
	// another execution should not alter any state
	thread.state = 0;
	TEST_ASSERT_FALSE(thread.run());
	TEST_ASSERT_EQUALS(thread.state, 0);
}

// ----------------------------------------------------------------------------
class TestingThreadRestart : public xpcc::pt::Protothread
{
public:
	TestingThreadRestart() :
		state(0),
		restartCondition(false)
	{
	}
	
	virtual bool
	run()
	{
		PT_BEGIN();
		
		this->state = 1;
		
		PT_YIELD();
		
		this->state = 2;
		
		if (this->restartCondition) {
			PT_RESTART();
		}
		
		this->state = 3;
		
		PT_YIELD();
		
		this->state = 4;
		
		PT_EXIT();
		
		// should never be executed!
		this->state = 5;
		
		PT_YIELD();
		PT_END();
	}
	
	uint8_t state;
	bool restartCondition;
};

void
ProtothreadTest::testRestartAndExit()
{
	TestingThreadRestart thread;
	thread.restartCondition = true;
	
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 1);
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 2);
	
	// restart condition is set, therefore the next executing should start
	// from the beginning
	thread.run();
	TEST_ASSERT_EQUALS(thread.state, 1);
	
	thread.restartCondition = false;
	TEST_ASSERT_TRUE(thread.run());
	TEST_ASSERT_EQUALS(thread.state, 3);
	
	// thread runs into a PT_EXIT()
	TEST_ASSERT_FALSE(thread.run());
	TEST_ASSERT_EQUALS(thread.state, 4);
	TEST_ASSERT_FALSE(thread.isRunning());
}

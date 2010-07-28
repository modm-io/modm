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
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <xpcc/workflow/protothread.hpp>

#include "protothread_test.hpp"

// ----------------------------------------------------------------------------
class TestingEmptyThread : public xpcc::Protothread
{
public:
	virtual bool
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
class TestingChildThread : public xpcc::Protothread
{
public:
	TestingChildThread() :
		state(0), condition(false)
	{
	}
	
	virtual bool
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

class TestingThread : public xpcc::Protothread
{
public:
	TestingThread() :
		state(0), condition(false),
		restartCondition(true)
	{
	}
	
	virtual bool
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
class TestingThreadRestart : public xpcc::Protothread
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

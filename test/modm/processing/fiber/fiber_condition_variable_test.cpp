/*
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "fiber_condition_variable_test.hpp"
#include "shared.hpp"
#include <modm/architecture/interface/fiber/condition_variable.hpp>

static struct
{
	uint8_t lock_count{0}, unlock_count{0};
	void inline reset() { lock_count = 0; unlock_count = 0; }
	void inline lock() { lock_count++; }
	void inline unlock() { unlock_count++; }
} elock;

bool predicate_value{false};
static bool predicate() { return predicate_value; }

void
ConditionVariableTest::setUp()
{
	state = 0;
	elock.reset();
	predicate_value = false;
}

// ========================== CONDITION VARIABLE WAIT =========================
static modm::fiber::condition_variable cv;

static void
f1()
{
	TEST_ASSERT_EQUALS(state++, 0u);

	cv.wait(elock); // goto 1

	TEST_ASSERT_EQUALS(elock.lock_count, 1u);
	TEST_ASSERT_EQUALS(elock.unlock_count, 1u);

	TEST_ASSERT_EQUALS(state++, 3u);
}

static void
f2()
{
	TEST_ASSERT_EQUALS(state++, 1u);
	// let f1 wait for a while
	modm::this_fiber::yield();
	modm::this_fiber::yield();
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 2u);
	cv.notify_one();
	modm::this_fiber::yield(); // goto 3

	TEST_ASSERT_EQUALS(state++, 4u);
}

void
ConditionVariableTest::testConditionVariableWait()
{
	modm::fiber::Task fiber1(stack1, f1), fiber2(stack2, f2);
	modm::fiber::Scheduler::run();
}

// ===================== CONDITION VARIABLE WAIT PREDICATE ====================
static void
f3()
{
	TEST_ASSERT_EQUALS(state++, 0u);

	cv.wait(elock, predicate); // goto 1

	TEST_ASSERT_EQUALS(elock.lock_count, 4u);
	TEST_ASSERT_EQUALS(elock.unlock_count, 4u);

	TEST_ASSERT_EQUALS(state++, 4u);
}

static void
f4()
{
	TEST_ASSERT_EQUALS(state++, 1u);
	// let f3 wait for a while
	modm::this_fiber::yield();
	modm::this_fiber::yield();
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 2u);
	cv.notify_one();
	modm::this_fiber::yield();
	cv.notify_any();
	modm::this_fiber::yield();
	cv.notify_one();
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 3u);
	predicate_value = true;
	cv.notify_one();
	modm::this_fiber::yield(); // goto 4

	TEST_ASSERT_EQUALS(state++, 5u);
}

void
ConditionVariableTest::testConditionVariableWaitPredicate()
{
	modm::fiber::Task fiber1(stack1, f3), fiber2(stack2, f4);
	modm::fiber::Scheduler::run();
}

// ===================== CONDITION VARIABLE WAIT PREDICATE ====================
static modm::fiber::stop_state stop;

static void
f5()
{
	TEST_ASSERT_EQUALS(state++, 0u);

	cv.wait(elock, stop.get_token(), predicate); // goto 1

	TEST_ASSERT_EQUALS(elock.lock_count, 4u);
	TEST_ASSERT_EQUALS(elock.unlock_count, 4u);

	TEST_ASSERT_EQUALS(state++, 4u);
}

static void
f6()
{
	TEST_ASSERT_EQUALS(state++, 1u);
	// let f3 wait for a while
	modm::this_fiber::yield();
	modm::this_fiber::yield();
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 2u);
	cv.notify_one();
	modm::this_fiber::yield();
	cv.notify_any();
	modm::this_fiber::yield();
	cv.notify_one();
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 3u);
	stop.request_stop();
	cv.notify_one();
	modm::this_fiber::yield(); // goto 4

	TEST_ASSERT_EQUALS(state++, 5u);
}

void
ConditionVariableTest::testConditionVariableWaitStopTokenPredicate()
{
	modm::fiber::Task fiber1(stack1, f5), fiber2(stack2, f6);
	modm::fiber::Scheduler::run();
}

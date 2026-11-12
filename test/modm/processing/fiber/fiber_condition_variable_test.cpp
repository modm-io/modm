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
#include <modm/architecture/interface/fiber/mutex.hpp>
#include <modm-test/mock/clock.hpp>

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
	cv.notify_all();
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
	cv.notify_all();
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

// ========================= CONDITION VARIABLE NOTIFY =========================
static struct
{
	// e.g. an interrupt notifies right after unlocking
	void inline lock() {}
	void inline unlock() { cv.notify_one(); }
} nlock;

void
ConditionVariableTest::testConditionVariableNotifyOnUnlock()
{
	using namespace std::chrono_literals;
	TEST_ASSERT_TRUE(cv.wait_for(nlock, 0ms) == modm::fiber::cv_status::no_timeout);
	TEST_ASSERT_TRUE(cv.wait_until(nlock, modm::chrono::milli_clock::now()) == modm::fiber::cv_status::no_timeout);
}

// A fiber that only notifies when the waiting fiber did not return in time
template< class Function >
static void
run_with_fallback(Function &&wait, modm::fiber::stop_state *stop = nullptr)
{
	bool done{false};
	modm::fiber::Task fiber1(stack1, [&]{ wait(); done = true; });
	modm::fiber::Task fiber2(stack2, [&]
	{
		modm::this_fiber::yield();
		if (stop) stop->request_stop(); // without notification
		for (uint8_t ii = 0; ii < 10; ii++) modm::this_fiber::yield();
		TEST_ASSERT_TRUE(done);
		while (not done) { cv.notify_one(); modm::this_fiber::yield(); }
	});
	modm::fiber::Scheduler::run();
}

void
ConditionVariableTest::testConditionVariableNotifyAfterPredicate()
{
	// e.g. an interrupt changes the state and notifies right after the
	// predicate was evaluated, the interrupt cannot hold the lock
	modm::fiber::mutex mtx;
	run_with_fallback([&]
	{
		uint8_t calls{0};
		mtx.lock();
		cv.wait(mtx, [&]{ if (calls++) return true; cv.notify_one(); return false; });
		mtx.unlock();
	});
}

void
ConditionVariableTest::testConditionVariableStopWakes()
{
	modm::fiber::mutex mtx;
	modm::fiber::stop_state stop_wait;
	run_with_fallback([&]
	{
		mtx.lock();
		TEST_ASSERT_FALSE(cv.wait(mtx, stop_wait.get_token(), []{ return false; }));
		mtx.unlock();
	}, &stop_wait);
}

void
ConditionVariableTest::testConditionVariableWaitForTotal()
{
	using namespace std::chrono_literals;
	using test_clock_ms = modm_test::chrono::milli_clock;
	test_clock_ms::enable();
	modm::fiber::mutex mtx;
	bool done{false};
	uint8_t notifications{0};
	modm::fiber::Task fiber1(stack1, [&]
	{
		mtx.lock();
		// the timeout is not restarted by notifications
		TEST_ASSERT_FALSE(cv.wait_for(mtx, 10ms, []{ return false; }));
		mtx.unlock();
		done = true;
	});
	modm::fiber::Task fiber2(stack2, [&]
	{
		for (; notifications < 100 and not done; notifications++)
		{
			cv.notify_one();
			test_clock_ms::increment(1);
			modm::this_fiber::yield();
		}
		while (not done) { test_clock_ms::increment(1); modm::this_fiber::yield(); }
	});
	modm::fiber::Scheduler::run();
	test_clock_ms::disable();
	TEST_ASSERT_TRUE(notifications <= 12);
}

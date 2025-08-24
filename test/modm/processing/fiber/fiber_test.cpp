/*
 * Copyright (c) 2020, Erik Henriksson
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "fiber_test.hpp"
#include "shared.hpp"

#include <modm/processing/timer.hpp>
#include <modm-test/mock/clock.hpp>

using namespace std::chrono_literals;
using test_clock_ms = modm_test::chrono::milli_clock;
using test_clock_us = modm_test::chrono::micro_clock;

void
FiberTest::setUp()
{
	test_clock_ms::enable();
	test_clock_us::enable();
	state = 0;
}

void
FiberTest::tearDown()
{
	test_clock_ms::disable();
	test_clock_us::disable();
}

// ================================== FIBER ===================================
static void
f1()
{
	TEST_ASSERT_EQUALS(state++, 0u);
	modm::this_fiber::yield();
	TEST_ASSERT_EQUALS(state++, 1u);
}

void
FiberTest::testOneFiber()
{
	modm::fiber::Task fiber(stack1, f1);
	TEST_ASSERT_DIFFERS(fiber.get_id(), modm::fiber::id(0));
	TEST_ASSERT_TRUE(fiber.joinable());
	modm::fiber::Scheduler::run();
	TEST_ASSERT_FALSE(fiber.joinable());
}

void
FiberTest::testTwoFibers()
{
	modm::fiber::Task fiber1(stack1, []
	{
		TEST_ASSERT_EQUALS(state++, 0u);
		modm::this_fiber::yield(); // goto 1
		TEST_ASSERT_EQUALS(state++, 2u);
		modm::this_fiber::yield();
		TEST_ASSERT_EQUALS(state++, 3u);
		modm::this_fiber::yield();
		TEST_ASSERT_EQUALS(state++, 4u);
		modm::this_fiber::yield();
		TEST_ASSERT_EQUALS(state++, 5u);
	});
	modm::fiber::Task fiber2(stack2, [&]
	{
		TEST_ASSERT_EQUALS(state++, 1u);
		TEST_ASSERT_TRUE(fiber1.joinable());
		TEST_ASSERT_FALSE(fiber2.joinable());
		fiber1.join(); // goto 2
		TEST_ASSERT_EQUALS(state++, 6u);
		fiber2.join(); // should not hang
		TEST_ASSERT_EQUALS(state++, 7u);
	});
	modm::fiber::Scheduler::run();
}

static __attribute__((noinline)) void
subroutine()
{
	TEST_ASSERT_EQUALS(state++, 2u);
	modm::this_fiber::yield(); // goto 3
	TEST_ASSERT_EQUALS(state++, 4u);
}

void
FiberTest::testYieldFromSubroutine()
{
	modm::fiber::Task fiber1(stack1, []
	{
		TEST_ASSERT_EQUALS(state++, 0u);
		modm::this_fiber::yield(); // goto 1
		TEST_ASSERT_EQUALS(state++, 3u);
	});
	modm::fiber::Task fiber2(stack2, [&]
	{
		TEST_ASSERT_EQUALS(state++, 1u);
		TEST_ASSERT_TRUE(fiber1.joinable());
		subroutine();
		TEST_ASSERT_FALSE(fiber1.joinable());
		TEST_ASSERT_EQUALS(state++, 5u);
	});
	modm::fiber::Scheduler::run();
}


void
FiberTest::testPollFor()
{
	test_clock_ms::setTime(1251);
	TEST_ASSERT_TRUE(modm::this_fiber::poll_for(20ms, []{ return true; }));
	// timeout is tested in the SleepFor() test
}


void
FiberTest::testPollUntil()
{
	test_clock_ms::setTime(451250);
	TEST_ASSERT_TRUE(modm::this_fiber::poll_until(modm::Clock::now() + 20ms, []{ return true; }));
	TEST_ASSERT_TRUE(modm::this_fiber::poll_until(modm::Clock::now() - 20ms, []{ return true; }));
	// timeout is tested in the SleepUntil() tests
}


static void
f4()
{
	TEST_ASSERT_EQUALS(state++, 0u);
	// yields only once
	modm::this_fiber::sleep_for(0s); // goto 1

	TEST_ASSERT_EQUALS(state++, 2u);
	// rounds up to 1us, not 0us!
	modm::this_fiber::sleep_for(100ns); // goto 3

	TEST_ASSERT_EQUALS(state++, 5u);
	modm::this_fiber::sleep_for(50ms); // goto 6
	TEST_ASSERT_EQUALS(state++, 10u);
}

static void
f5()
{
	TEST_ASSERT_EQUALS(state++, 1u);
	modm::this_fiber::yield(); // goto 2

	TEST_ASSERT_EQUALS(state++, 3u);
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 4u);
	test_clock_us::increment(1);
	modm::this_fiber::yield(); // goto 5

	TEST_ASSERT_EQUALS(state++, 6u);
	test_clock_ms::increment(10);
	TEST_ASSERT_EQUALS(state++, 7u);
	modm::this_fiber::yield();

	test_clock_ms::increment(20);
	TEST_ASSERT_EQUALS(state++, 8u);
	modm::this_fiber::yield();

	test_clock_ms::increment(30);
	TEST_ASSERT_EQUALS(state++, 9u);
	modm::this_fiber::yield(); // goto 10

	TEST_ASSERT_EQUALS(state++, 11u);
}

static void
runSleepFor(uint32_t startTime)
{
	test_clock_ms::setTime(startTime);
	test_clock_us::setTime(startTime);
	modm::fiber::Task fiber1(stack1, f4), fiber2(stack2, f5);
	modm::fiber::Scheduler::run();
}


void
FiberTest::testSleepFor()
{
	runSleepFor(16203);
	state = 0;
	runSleepFor(0xffff'ffff - 30);
}

static void
f6()
{
	TEST_ASSERT_EQUALS(state++, 1u);
	test_clock_ms::increment(10);
	TEST_ASSERT_EQUALS(state++, 2u);
	modm::this_fiber::yield();

	test_clock_ms::increment(20);
	TEST_ASSERT_EQUALS(state++, 3u);
	modm::this_fiber::yield();

	test_clock_ms::increment(30);
	TEST_ASSERT_EQUALS(state++, 4u);
	modm::this_fiber::yield(); // goto 5

	TEST_ASSERT_EQUALS(state++, 6u);
}

static void
f7()
{
	TEST_ASSERT_EQUALS(state++, 0u); // goto 1
	modm::this_fiber::sleep_until(modm::Clock::now() + 50ms);
	TEST_ASSERT_EQUALS(state++, 5u);
	modm::this_fiber::yield(); // goto 6
	modm::this_fiber::sleep_until(modm::Clock::now() - 50ms);
	TEST_ASSERT_EQUALS(state++, 7u);
}

static void
runSleepUntil(uint32_t startTime)
{
	test_clock_ms::setTime(startTime);
	modm::fiber::Task fiber1(stack1, f7), fiber2(stack2, f6);
	modm::fiber::Scheduler::run();
}

void
FiberTest::testSleepUntil()
{
	runSleepUntil(1502);
	state = 0;
	runSleepUntil(0xffff'ffff - 30);
}

static void
f8(modm::fiber::stop_token stoken)
{
	TEST_ASSERT_EQUALS(state++, 0u);
	TEST_ASSERT_TRUE(stoken.stop_possible());
	TEST_ASSERT_FALSE(stoken.stop_requested());
	while(not stoken.stop_requested())
	{
		modm::this_fiber::yield(); // goto 1
		state++; // 2, 4
	}
	TEST_ASSERT_TRUE(stoken.stop_requested());
	TEST_ASSERT_EQUALS(state++, 5u);
}

void
FiberTest::testStopToken()
{
	modm::fiber::Task fiber1(stack1, f8);
	modm::fiber::Task fiber2(stack2, [&](modm::fiber::stop_token stoken)
	{
		TEST_ASSERT_EQUALS(state++, 1u);
		TEST_ASSERT_TRUE(stoken.stop_possible());
		TEST_ASSERT_FALSE(stoken.stop_requested());
		modm::this_fiber::yield(); // goto 2
		TEST_ASSERT_EQUALS(state++, 3u);
		fiber1.request_stop();
		modm::this_fiber::yield(); // goto 4
		TEST_ASSERT_EQUALS(state++, 6u);
	});
	modm::fiber::Scheduler::run();
}

void
FiberTest::testTimeoutWait()
{
	modm::ShortTimeout timeout(20ms);
	modm::fiber::Task fiber1(stack1, [&]
	{
		TEST_ASSERT_EQUALS(state++, 0u);

		timeout.wait();
		TEST_ASSERT_EQUALS(state++, 5u);

	});
	modm::fiber::Task fiber2(stack2, [&]
	{
		TEST_ASSERT_EQUALS(state++, 1u);
		modm::this_fiber::yield();
		TEST_ASSERT_EQUALS(state++, 2u);
		test_clock_ms::increment(1);
		modm::this_fiber::yield();
		TEST_ASSERT_EQUALS(state++, 3u);
		test_clock_ms::increment(10);
		modm::this_fiber::yield();
		TEST_ASSERT_EQUALS(state++, 4u);
		test_clock_ms::increment(30);
		modm::this_fiber::yield(); // goto 3

		TEST_ASSERT_EQUALS(state++, 6u);
	});
	modm::fiber::Scheduler::run();
}

void
FiberTest::testPeriodicTimerWait()
{
	modm::ShortPeriodicTimer timer(20ms);
	modm::fiber::Task fiber1(stack1, [&]
	{
		TEST_ASSERT_EQUALS(state++, 0u);

		TEST_ASSERT_EQUALS(timer.wait(), 1u);
		TEST_ASSERT_EQUALS(state++, 3u);

		TEST_ASSERT_EQUALS(timer.wait(), 1u); // goto 4
		TEST_ASSERT_EQUALS(state++, 6u);

		TEST_ASSERT_EQUALS(timer.wait(), 3u); // goto 7
		TEST_ASSERT_EQUALS(state++, 9u);

	});
	modm::fiber::Task fiber2(stack2, [&]
	{
		TEST_ASSERT_EQUALS(state++, 1u);
		modm::this_fiber::yield();
		TEST_ASSERT_EQUALS(state++, 2u);
		test_clock_ms::increment(30);
		modm::this_fiber::yield(); // goto 3

		TEST_ASSERT_EQUALS(state++, 4u);
		test_clock_ms::increment(9);
		modm::this_fiber::yield();
		TEST_ASSERT_EQUALS(state++, 5u);
		test_clock_ms::increment(1);
		modm::this_fiber::yield(); // goto 6

		TEST_ASSERT_EQUALS(state++, 7u);
		test_clock_ms::increment(15);
		modm::this_fiber::yield();
		TEST_ASSERT_EQUALS(state++, 8u);
		test_clock_ms::increment(50);
		modm::this_fiber::yield(); // goto 9

		TEST_ASSERT_EQUALS(state++, 10u);
	});
	modm::fiber::Scheduler::run();
}

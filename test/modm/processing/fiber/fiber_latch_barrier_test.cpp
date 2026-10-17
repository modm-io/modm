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

#include "fiber_latch_barrier_test.hpp"
#include "shared.hpp"
#include <modm/architecture/interface/fiber/latch.hpp>
#include <modm/architecture/interface/fiber/barrier.hpp>

void
LatchBarrierTest::setUp()
{
	state = 0;
}

// ================================== LATCH ===================================
void
LatchBarrierTest::testLatch0()
{
	modm::fiber::latch latch{0};
	TEST_ASSERT_TRUE(latch.try_wait());
	latch.count_down(1);
	TEST_ASSERT_TRUE(latch.try_wait());
	latch.count_down(100);
	TEST_ASSERT_TRUE(latch.try_wait());
}

void
LatchBarrierTest::testLatch1()
{
	modm::fiber::latch latch{1};
	TEST_ASSERT_FALSE(latch.try_wait());
	latch.count_down(1);
	TEST_ASSERT_TRUE(latch.try_wait());
	latch.count_down(100);
	TEST_ASSERT_TRUE(latch.try_wait());
}

void
LatchBarrierTest::testLatch2()
{
	modm::fiber::latch latch{2};
	TEST_ASSERT_FALSE(latch.try_wait());
	latch.count_down(1);
	TEST_ASSERT_FALSE(latch.try_wait());
	latch.count_down(100);
	TEST_ASSERT_TRUE(latch.try_wait());
	latch.count_down(100);
	TEST_ASSERT_TRUE(latch.try_wait());
}

void
LatchBarrierTest::testLatch10()
{
	modm::fiber::latch latch{10};
	TEST_ASSERT_FALSE(latch.try_wait());
	latch.count_down(1);
	TEST_ASSERT_FALSE(latch.try_wait());
	latch.count_down(100);
	TEST_ASSERT_TRUE(latch.try_wait());
	latch.count_down(100);
	TEST_ASSERT_TRUE(latch.try_wait());
}

static modm::fiber::latch ltc{3};

static void
f1()
{
	TEST_ASSERT_EQUALS(state++, 0u);
	TEST_ASSERT_FALSE(ltc.try_wait());

	ltc.wait(); // goto 1

	TEST_ASSERT_TRUE(ltc.try_wait());
	TEST_ASSERT_EQUALS(state++, 4u);
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
	ltc.count_down();
	TEST_ASSERT_FALSE(ltc.try_wait());
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 3u);
	ltc.count_down(2);
	TEST_ASSERT_TRUE(ltc.try_wait());
	modm::this_fiber::yield(); // goto 4

	TEST_ASSERT_EQUALS(state++, 5u);
}

void
LatchBarrierTest::testLatchWait()
{
	modm::fiber::Task fiber1(stack1, f1), fiber2(stack2, f2);
	modm::fiber::Scheduler::run();
}

// ================================= BARRIER ==================================

static modm::fiber::id completion_id;
static void on_completion()
{
	state++;
	completion_id = modm::this_fiber::get_id();
}

void
LatchBarrierTest::testBarrier()
{
	modm::fiber::barrier bar{2, on_completion};

	TEST_ASSERT_EQUALS(bar.arrive(0), 0u);
	TEST_ASSERT_EQUALS(state, 0u);
	TEST_ASSERT_EQUALS(bar.arrive(), 0u);
	TEST_ASSERT_EQUALS(state, 0u);
	TEST_ASSERT_EQUALS(bar.arrive(1), 0u);
	TEST_ASSERT_EQUALS(state, 1u);
	TEST_ASSERT_EQUALS(completion_id, 0u);

	TEST_ASSERT_EQUALS(bar.arrive(2), 1u);
	TEST_ASSERT_EQUALS(state, 2u);

	TEST_ASSERT_EQUALS(bar.arrive(10), 2u);
	TEST_ASSERT_EQUALS(state, 3u);

	bar.arrive_and_drop(); // expected=1
	TEST_ASSERT_EQUALS(state, 3u);
	TEST_ASSERT_EQUALS(bar.arrive(), 3u);
	TEST_ASSERT_EQUALS(state, 4u);

	TEST_ASSERT_EQUALS(bar.arrive(), 4u);
	TEST_ASSERT_EQUALS(state, 5u);

	bar.arrive_and_drop(); // expected=0
	TEST_ASSERT_EQUALS(state, 6u);

	TEST_ASSERT_EQUALS(bar.arrive(), 6u);
	TEST_ASSERT_EQUALS(state, 7u);
}


static modm::fiber::barrier brr{2, on_completion};
static modm::fiber::id f3_id, f4_id;

static void
f3()
{
	TEST_ASSERT_EQUALS(state++, 0u);
	const auto token = brr.arrive();
	TEST_ASSERT_EQUALS(token, 0u);

	TEST_ASSERT_EQUALS(state++, 1u);
	brr.wait(token); // goto 2
	TEST_ASSERT_EQUALS(completion_id, f4_id);

	const auto token2 = brr.arrive();
	// on_completion() called: state++
	TEST_ASSERT_EQUALS(token2, 1u);

	TEST_ASSERT_EQUALS(state++, 7u);
	brr.wait(token2); // does not wait
	TEST_ASSERT_EQUALS(completion_id, f3_id);

	TEST_ASSERT_EQUALS(state++, 8u);
}

static void
f4()
{
	TEST_ASSERT_EQUALS(state++, 2u);
	// let f3 wait for a while
	modm::this_fiber::yield();
	modm::this_fiber::yield();
	modm::this_fiber::yield();

	const auto token = brr.arrive();
	// on_completion() called: state++
	TEST_ASSERT_EQUALS(token, 0u);

	TEST_ASSERT_EQUALS(state++, 4u);
	brr.wait(token); // does not wait
	TEST_ASSERT_EQUALS(completion_id, f4_id);

	const auto token2 = brr.arrive();
	TEST_ASSERT_EQUALS(token2, 1u);

	TEST_ASSERT_EQUALS(state++, 5u);
	brr.wait(token2); // goto 6
	TEST_ASSERT_EQUALS(completion_id, f3_id);

	TEST_ASSERT_EQUALS(state++, 9u);
}


void
LatchBarrierTest::testBarrierWait()
{
	modm::fiber::Task fiber1(stack1, f3), fiber2(stack2, f4);
	f3_id = fiber1.get_id();
	f4_id = fiber2.get_id();
	modm::fiber::Scheduler::run();
}

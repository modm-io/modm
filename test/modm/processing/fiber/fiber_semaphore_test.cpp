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

#include "fiber_semaphore_test.hpp"
#include "shared.hpp"
#include <modm/architecture/interface/fiber/semaphore.hpp>

void
FiberSemaphoreTest::setUp()
{
	state = 0;
}

// ================================== MUTEX ===================================
static modm::fiber::counting_semaphore sem{3};

static void
f1()
{
	TEST_ASSERT_EQUALS(state++, 0u);
	TEST_ASSERT_TRUE(sem.try_acquire()); // 2
	TEST_ASSERT_TRUE(sem.try_acquire()); // 1
	TEST_ASSERT_TRUE(sem.try_acquire()); // 0
	TEST_ASSERT_FALSE(sem.try_acquire()); // 0
	TEST_ASSERT_FALSE(sem.try_acquire()); // 0
	sem.release(); // 1
	sem.acquire(); // 0
	TEST_ASSERT_EQUALS(state++, 1u);
	sem.acquire(); // goto 2, 0

	TEST_ASSERT_EQUALS(state++, 4u);
	sem.release(); // 1
	sem.release(); // 2
	modm::this_fiber::yield(); // goto 5

	TEST_ASSERT_EQUALS(state++, 6u);
	sem.acquire();
	sem.acquire(); // goto 7, 0

	TEST_ASSERT_EQUALS(state++, 9u);
}

static void
f2()
{
	TEST_ASSERT_EQUALS(state++, 2u);
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 3u);
	sem.release(); // 1
	modm::this_fiber::yield(); // goto 4

	TEST_ASSERT_EQUALS(state++, 5u);
	sem.acquire(); // 1
	modm::this_fiber::yield(); // goto 6

	TEST_ASSERT_EQUALS(state++, 7u);
	sem.release(); // 1
	sem.release(); // 2
	sem.release(); // 3

	TEST_ASSERT_EQUALS(state++, 8u);
}

void
FiberSemaphoreTest::testCountingSemaphore()
{
	// should not block
	TEST_ASSERT_TRUE(sem.try_acquire());
	TEST_ASSERT_TRUE(sem.try_acquire());
	TEST_ASSERT_TRUE(sem.try_acquire());
	TEST_ASSERT_FALSE(sem.try_acquire());
	TEST_ASSERT_FALSE(sem.try_acquire());
	sem.release();
	// shouldn't block without scheduler
	sem.acquire();
	sem.release();
	sem.release();
	sem.release();

	modm::fiber::Task fiber1(stack1, f1), fiber2(stack2, f2);
	modm::fiber::Scheduler::run();
}


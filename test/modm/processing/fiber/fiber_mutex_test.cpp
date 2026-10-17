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

#include "fiber_mutex_test.hpp"
#include "shared.hpp"
#include <modm/architecture/interface/fiber/mutex.hpp>
#include <modm/architecture/interface/fiber/shared_mutex.hpp>

void
FiberMutexTest::setUp()
{
	state = 0;
}

// ================================== MUTEX ===================================
static modm::fiber::mutex mtx;

static void
f1()
{
	TEST_ASSERT_EQUALS(state++, 0u);
	TEST_ASSERT_TRUE(mtx.try_lock());
	TEST_ASSERT_FALSE(mtx.try_lock());
	TEST_ASSERT_FALSE(mtx.try_lock());
	mtx.unlock();
	mtx.unlock();

	TEST_ASSERT_EQUALS(state++, 1u);
	mtx.lock(); // should not yield
	TEST_ASSERT_EQUALS(state++, 2u);
	mtx.lock(); // goto 3

	mtx.unlock();
	mtx.unlock();
	TEST_ASSERT_EQUALS(state++, 5u);
	mtx.lock(); // should not yield
	TEST_ASSERT_EQUALS(state++, 6u);
	mtx.lock(); // goto 7

	TEST_ASSERT_EQUALS(state++, 8u);
}

static void
f2()
{
	TEST_ASSERT_EQUALS(state++, 3u);
	// let f1 wait for a while
	modm::this_fiber::yield();
	modm::this_fiber::yield();
	modm::this_fiber::yield();
	TEST_ASSERT_EQUALS(state++, 4u);
	mtx.unlock();
	modm::this_fiber::yield(); // goto 5

	TEST_ASSERT_EQUALS(state++, 7u);
	mtx.unlock();
	modm::this_fiber::yield(); // goto 8
	TEST_ASSERT_EQUALS(state++, 9u);
}

void
FiberMutexTest::testMutex()
{
	// should not block
	TEST_ASSERT_TRUE(mtx.try_lock());
	TEST_ASSERT_FALSE(mtx.try_lock());
	TEST_ASSERT_FALSE(mtx.try_lock());
	mtx.unlock();
	// multiple unlock calls should be fine too
	mtx.unlock();
	mtx.unlock();
	mtx.unlock();
	// shouldn't block without scheduler
	mtx.lock();
	mtx.unlock();

	modm::fiber::Task fiber1(stack1, f1), fiber2(stack2, f2);
	modm::fiber::Scheduler::run();
}

// ============================== RECURSIVE MUTEX =============================
static modm::fiber::recursive_mutex rc_mtx;

static void
f3()
{
	TEST_ASSERT_EQUALS(state++, 0u);

	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	modm::this_fiber::yield(); // goto 1

	TEST_ASSERT_EQUALS(state++, 3u);
	rc_mtx.unlock();
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 4u);
	rc_mtx.unlock();
	modm::this_fiber::yield();

	TEST_ASSERT_EQUALS(state++, 5u);
	rc_mtx.unlock();
	rc_mtx.unlock(); // more than necessary
	rc_mtx.unlock();
	rc_mtx.unlock();
	modm::this_fiber::yield(); // goto 6

	TEST_ASSERT_EQUALS(state++, 7u);
	rc_mtx.lock(); // goto 8

	TEST_ASSERT_EQUALS(state++, 11u);
	rc_mtx.unlock();
	rc_mtx.unlock();

	TEST_ASSERT_EQUALS(state++, 12u);
}

static void
f4()
{
	TEST_ASSERT_EQUALS(state++, 1u);
	TEST_ASSERT_FALSE(rc_mtx.try_lock());
	TEST_ASSERT_FALSE(rc_mtx.try_lock());
	TEST_ASSERT_FALSE(rc_mtx.try_lock());

	TEST_ASSERT_EQUALS(state++, 2u);
	rc_mtx.lock(); // goto 3

	TEST_ASSERT_EQUALS(state++, 6u);
	rc_mtx.lock();
	rc_mtx.lock();
	modm::this_fiber::yield(); // goto 7

	TEST_ASSERT_EQUALS(state++, 8u);
	rc_mtx.unlock();
	modm::this_fiber::yield();
	TEST_ASSERT_EQUALS(state++, 9u);
	rc_mtx.unlock();
	modm::this_fiber::yield();
	TEST_ASSERT_EQUALS(state++, 10u);
	rc_mtx.unlock();
	modm::this_fiber::yield(); // goto 11

	TEST_ASSERT_EQUALS(state++, 13u);
}

void
FiberMutexTest::testRecursiveMutex()
{
	// this should also work without scheduler, since fiber id is zero then
	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	rc_mtx.unlock();
	rc_mtx.unlock();
	rc_mtx.unlock();
	// more unlocks should be fine
	rc_mtx.unlock();
	rc_mtx.unlock();

	// should not block either without scheduler
	rc_mtx.lock();
	rc_mtx.lock();
	rc_mtx.lock();
	rc_mtx.unlock();
	rc_mtx.unlock();
	rc_mtx.unlock();
	rc_mtx.unlock();

	modm::fiber::Task fiber1(stack1, f3), fiber2(stack2, f4);
	modm::fiber::Scheduler::run();

	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	rc_mtx.unlock();
	rc_mtx.unlock();
}

// =============================== SHARED MUTEX ===============================
static modm::fiber::shared_mutex sh_mtx;

static void
f5()
{
	TEST_ASSERT_EQUALS(state++, 0u);
	// get the exclusive lock
	sh_mtx.lock();
	TEST_ASSERT_FALSE(sh_mtx.try_lock());
	modm::this_fiber::yield(); // goto 1

	TEST_ASSERT_EQUALS(state++, 2u);
	sh_mtx.unlock();
	modm::this_fiber::yield(); // goto 3

	TEST_ASSERT_EQUALS(state++, 4u);
	// get the shared lock
	sh_mtx.lock_shared();
	sh_mtx.lock_shared();
	modm::this_fiber::yield(); // goto 5

	TEST_ASSERT_EQUALS(state++, 6u);
	modm::this_fiber::yield();
	modm::this_fiber::yield();
	modm::this_fiber::yield();
	// still locked
	sh_mtx.unlock_shared();
	modm::this_fiber::yield(); // goto 7

	TEST_ASSERT_EQUALS(state++, 9u);
}

static void
f6()
{
	TEST_ASSERT_EQUALS(state++, 1u);
	// cannot get exclusive lock
	sh_mtx.lock(); // goto 2

	TEST_ASSERT_EQUALS(state++, 3u);
	TEST_ASSERT_FALSE(sh_mtx.try_lock());
	sh_mtx.unlock();
	modm::this_fiber::yield(); // goto 4

	TEST_ASSERT_EQUALS(state++, 5u);
	// can get shared lock
	sh_mtx.lock_shared();
	sh_mtx.lock_shared();
	// cannot get exclusive lock
	sh_mtx.lock(); // goto 6

	TEST_ASSERT_EQUALS(state++, 7u);
	sh_mtx.unlock();

	TEST_ASSERT_EQUALS(state++, 8u);
}

void
FiberMutexTest::testSharedMutex()
{
	TEST_ASSERT_TRUE(sh_mtx.try_lock());
	TEST_ASSERT_FALSE(sh_mtx.try_lock());
	TEST_ASSERT_FALSE(sh_mtx.try_lock());
	sh_mtx.unlock();
	// more unlocks should be fine
	sh_mtx.unlock();
	sh_mtx.unlock();

	TEST_ASSERT_TRUE(sh_mtx.try_lock_shared());
	TEST_ASSERT_TRUE(sh_mtx.try_lock_shared());
	TEST_ASSERT_TRUE(sh_mtx.try_lock_shared());
	sh_mtx.unlock();
	// more unlocks should be fine
	sh_mtx.unlock();
	sh_mtx.unlock();


	modm::fiber::Task fiber1(stack1, f5), fiber2(stack2, f6);
	modm::fiber::Scheduler::run();

	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	rc_mtx.unlock();
	rc_mtx.unlock();
}

// =============================== TIMED MUTEX ================================
// =========================== TIMED RECURSIVE MUTEX ==========================
// ============================ TIMED SHARED MUTEX ============================
// All implementations only add poll_for/_until with try_lock*() function.
// Explicit tests are omitted, since they are tested in FiberTest::testSleep*().

void
FiberMutexTest::testCallOnce()
{
	modm::fiber::once_flag flag;
	uint8_t ii = 0;
	while(++ii < 10) modm::fiber::call_once(flag, []{ state++; });
	modm::fiber::call_once(flag, [&]{
		modm::fiber::call_once(flag, [&]{
			modm::fiber::call_once(flag, []{ state++; });
		});
	});
	TEST_ASSERT_EQUALS(ii, 10u);
	TEST_ASSERT_EQUALS(state, 1u);
}

/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "fiber_sync_test.hpp"
#include <modm/processing/fiber.hpp>
#include <modm/processing/fiber/mutex.hpp>
#include <modm/processing/fiber/shared_mutex.hpp>
#include <modm/processing/fiber/semaphore.hpp>
#include <modm/processing/fiber/latch.hpp>
#include <modm/processing/fiber/condition_variable.hpp>

void
FiberSyncTest::testPolling()
{
	TEST_ASSERT_EQUALS(modm::this_fiber::get_id(), modm::fiber::id(0));

	uint8_t count{0};
	modm::this_fiber::poll([&]{ return ++count >= 3; });
	TEST_ASSERT_EQUALS(count, 3);
}

void
FiberSyncTest::testMutex()
{
	modm::fiber::mutex mtx;
	mtx.lock();
	TEST_ASSERT_FALSE(mtx.try_lock());
	mtx.unlock();
	{
		modm::fiber::lock_guard guard{mtx};
		TEST_ASSERT_FALSE(mtx.try_lock());
	}
	TEST_ASSERT_TRUE(mtx.try_lock());
	mtx.unlock();

	modm::fiber::recursive_mutex rc_mtx;
	rc_mtx.lock();
	rc_mtx.lock();
	TEST_ASSERT_TRUE(rc_mtx.try_lock());
	rc_mtx.unlock();
	rc_mtx.unlock();
	rc_mtx.unlock();

	modm::fiber::shared_mutex sh_mtx;
	sh_mtx.lock_shared();
	TEST_ASSERT_TRUE(sh_mtx.try_lock_shared());
	TEST_ASSERT_FALSE(sh_mtx.try_lock());
	sh_mtx.unlock_shared();
	sh_mtx.lock();
	TEST_ASSERT_FALSE(sh_mtx.try_lock_shared());
	sh_mtx.unlock();

	uint8_t calls{0};
	modm::fiber::once_flag flag;
	modm::fiber::call_once(flag, [&]{ calls++; });
	modm::fiber::call_once(flag, [&]{ calls++; });
	TEST_ASSERT_EQUALS(calls, 1);
}

void
FiberSyncTest::testSemaphore()
{
	modm::fiber::counting_semaphore sem{2};
	sem.acquire();
	sem.acquire();
	TEST_ASSERT_FALSE(sem.try_acquire());
	sem.release();
	sem.acquire();
	TEST_ASSERT_FALSE(sem.try_acquire());
}

void
FiberSyncTest::testLatch()
{
	modm::fiber::latch ltc{2};
	TEST_ASSERT_FALSE(ltc.try_wait());
	ltc.count_down();
	TEST_ASSERT_FALSE(ltc.try_wait());
	ltc.arrive_and_wait();
	TEST_ASSERT_TRUE(ltc.try_wait());
}

void
FiberSyncTest::testConditionVariable()
{
	// std::unique_lock pulls in the exception handling of libstdc++, which
	// does not fit on small devices, so the mutex is used as lock directly.
	modm::fiber::mutex mtx;
	modm::fiber::condition_variable cv;
	modm::fiber::stop_state stop;
	mtx.lock();

	// predicate is already true, so this must not wait
	cv.wait(mtx, []{ return true; });
	TEST_ASSERT_FALSE(mtx.try_lock());

	stop.request_stop();
	TEST_ASSERT_FALSE(cv.wait(mtx, stop.get_token(), []{ return false; }));
	TEST_ASSERT_FALSE(mtx.try_lock());
	mtx.unlock();
}

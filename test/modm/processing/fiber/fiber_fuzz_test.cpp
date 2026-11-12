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

#include "fiber_fuzz_test.hpp"
#include "shared.hpp"
#include <modm/architecture/interface/fiber/mutex.hpp>
#include <modm/architecture/interface/fiber/shared_mutex.hpp>
#include <modm/architecture/interface/fiber/semaphore.hpp>
#include <modm/architecture/interface/fiber/barrier.hpp>
#include <modm/architecture/interface/fiber/condition_variable.hpp>

namespace
{

constexpr uint8_t Seeds{40};
constexpr uint8_t Steps{20};
uint32_t random_state;

uint32_t
next_random()
{
	// xorshift32, never zero
	random_state ^= random_state << 13;
	random_state ^= random_state >> 17;
	random_state ^= random_state << 5;
	return random_state;
}

void
random_yield()
{
	for (uint8_t count = next_random() % 4; count; count--)
		modm::this_fiber::yield();
}

/// Calls function(seed, fiber) in two fibers for every seed.
void
fuzz(auto &&function)
{
	for (uint8_t seed = 0; seed < Seeds; seed++)
	{
		random_state = (seed + 1) * 2654435761u;
		modm::fiber::Task fiber0(stack1, [&]{ function(seed, 0); });
		modm::fiber::Task fiber1(stack2, [&]{ function(seed, 1); });
		modm::fiber::Scheduler::run();
	}
}

}

void
FiberFuzzTest::testMutex()
{
	modm::fiber::mutex mutex;
	modm::fiber::recursive_mutex recursive;
	uint8_t inside{}, inside_recursive{};
	fuzz([&](uint8_t, uint8_t)
	{
		for (uint8_t step = 0; step < Steps; step++)
		{
			random_yield();
			mutex.lock();
			TEST_ASSERT_EQUALS(inside++, 0u);
			random_yield();
			inside--;
			mutex.unlock();

			recursive.lock();
			TEST_ASSERT_EQUALS(inside_recursive++, 0u);
			random_yield();
			recursive.lock();
			random_yield();
			recursive.unlock();
			random_yield();
			inside_recursive--;
			recursive.unlock();
		}
	});
}

void
FiberFuzzTest::testSharedMutex()
{
	modm::fiber::shared_mutex mutex;
	uint8_t readers{}, writers{};
	fuzz([&](uint8_t, uint8_t)
	{
		for (uint8_t step = 0; step < Steps; step++)
		{
			random_yield();
			if (next_random() & 1)
			{
				mutex.lock_shared();
				readers++;
				TEST_ASSERT_EQUALS(writers, 0u);
				random_yield();
				TEST_ASSERT_EQUALS(writers, 0u);
				readers--;
				mutex.unlock_shared();
			}
			else
			{
				mutex.lock();
				TEST_ASSERT_EQUALS(writers++, 0u);
				TEST_ASSERT_EQUALS(readers, 0u);
				random_yield();
				TEST_ASSERT_EQUALS(readers, 0u);
				writers--;
				mutex.unlock();
			}
		}
	});
}

void
FiberFuzzTest::testCallOnce()
{
	modm::fiber::once_flag flags[Seeds];
	bool done[Seeds]{};
	uint8_t calls{};
	fuzz([&](uint8_t seed, uint8_t)
	{
		random_yield();
		modm::fiber::call_once(flags[seed], [&]
		{
			random_yield();
			calls++;
			random_yield();
			done[seed] = true;
		});
		TEST_ASSERT_TRUE(done[seed]);
	});
	TEST_ASSERT_EQUALS(calls, Seeds);
}

void
FiberFuzzTest::testBarrier()
{
	uint16_t completions{};
	modm::fiber::barrier barrier{2, [&]{ random_yield(); completions++; }};
	fuzz([&](uint8_t seed, uint8_t)
	{
		for (uint8_t step = 0; step < Steps; step++)
		{
			random_yield();
			barrier.arrive_and_wait();
			// the completion of this phase has returned
			TEST_ASSERT_TRUE(completions >= uint16_t(seed * Steps + step + 1));
		}
	});
	TEST_ASSERT_EQUALS(completions, uint16_t(Seeds * Steps));
}

void
FiberFuzzTest::testSemaphore()
{
	modm::fiber::binary_semaphore semaphore{1};
	uint8_t inside{};
	fuzz([&](uint8_t, uint8_t)
	{
		for (uint8_t step = 0; step < Steps; step++)
		{
			random_yield();
			semaphore.acquire();
			TEST_ASSERT_EQUALS(inside++, 0u);
			random_yield();
			inside--;
			semaphore.release();
			// a spurious release, e.g. from an interrupt
			if (next_random() % 4 == 0) semaphore.release();
		}
	});
}

void
FiberFuzzTest::testConditionVariable()
{
	modm::fiber::mutex mutex;
	modm::fiber::condition_variable cv;
	uint8_t items{};
	uint16_t produced{}, consumed{};
	fuzz([&](uint8_t, uint8_t fiber)
	{
		for (uint8_t step = 0; step < Steps; step++)
		{
			random_yield();
			mutex.lock();
			if (fiber == 0)
			{
				items++;
				produced++;
				random_yield();
				mutex.unlock();
				cv.notify_one();
			}
			else
			{
				cv.wait(mutex, [&]{ return items > 0; });
				items--;
				consumed++;
				random_yield();
				mutex.unlock();
			}
		}
	});
	TEST_ASSERT_EQUALS(items, 0u);
	TEST_ASSERT_EQUALS(consumed, produced);
}

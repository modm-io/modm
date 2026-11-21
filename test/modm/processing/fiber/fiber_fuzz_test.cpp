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
// FNV-1a over which fiber resumes after every yield. All fiber implementations
// must schedule identically, so this value is the same on every target!
uint32_t schedule;
modm::fiber::id first_fiber;

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
	{
		modm::this_fiber::yield();
		schedule = (schedule ^ uint8_t(modm::this_fiber::get_id() == first_fiber)) * 16777619ul;
	}
}

/// Calls function(seed, fiber) in two fibers for every seed and compares the
/// resulting schedule.
void
fuzz(uint32_t expected_schedule, auto &&function)
{
	schedule = 2166136261ul;
	for (uint8_t seed = 0; seed < Seeds; seed++)
	{
		random_state = (seed + 1) * 2654435761ul;
		modm::fiber::Task fiber0(stack1, [&]{ function(seed, 0); });
		modm::fiber::Task fiber1(stack2, [&]{ function(seed, 1); });
		first_fiber = fiber0.get_id();
		modm::fiber::Scheduler::run();
	}
	TEST_ASSERT_EQUALS(schedule, expected_schedule);
}

}

void
FiberFuzzTest::testMutex()
{
	modm::fiber::mutex mutex;
	modm::fiber::recursive_mutex recursive;
	uint8_t inside{}, inside_recursive{};
	fuzz(3265865098ul, [&](uint8_t, uint8_t)
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
	fuzz(3560066811ul, [&](uint8_t, uint8_t)
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
	fuzz(1024557508ul, [&](uint8_t seed, uint8_t)
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
	fuzz(1661557593ul, [&](uint8_t seed, uint8_t)
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
	fuzz(1222929258ul, [&](uint8_t, uint8_t)
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
	fuzz(2017731572ul, [&](uint8_t, uint8_t fiber)
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

#ifdef MODM_CONTEXT_TLS
namespace
{
uint16_t tls_constructed;
uint8_t tls_errors;
struct TlsObject
{
	uint32_t pattern{0};
	TlsObject() { tls_constructed++; }
};
constinit thread_local uint32_t tls_words[8];
alignas(16) constinit thread_local uint8_t tls_aligned{0x5A};
thread_local TlsObject tls_object;
}
#endif

void
FiberFuzzTest::testThreadLocal()
{
#ifdef MODM_CONTEXT_TLS
	tls_constructed = tls_errors = 0;
	fuzz(2872973855ul, [&](uint8_t seed, uint8_t fiber)
	{
		// every fiber starts with fresh storage
		for (auto word : tls_words) if (word) tls_errors++;
		if (tls_aligned != 0x5A or uintptr_t(&tls_aligned) % 16) tls_errors++;
		const uint32_t pattern = (seed << 8) | (fiber + 1);
		tls_object.pattern = pattern;
		for (uint8_t step = 0; step < Steps; step++)
		{
			tls_words[next_random() % 8] = pattern ^ step;
			random_yield();
			// no other fiber wrote into this storage
			for (auto word : tls_words) if (word and (word ^ pattern) >= Steps) tls_errors++;
			if (tls_object.pattern != pattern) tls_errors++;
		}
		TEST_ASSERT_EQUALS(tls_errors, 0u);
	});
	// each fiber constructed its own object
	TEST_ASSERT_EQUALS(tls_constructed, uint16_t(2 * Seeds));
	TEST_ASSERT_EQUALS(tls_errors, 0u);
#endif
}

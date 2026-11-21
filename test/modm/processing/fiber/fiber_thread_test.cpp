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

#include "fiber_thread_test.hpp"
#include "shared.hpp"
#include <modm/architecture/interface/fiber/mutex.hpp>
#include <modm/architecture/interface/fiber/shared_mutex.hpp>
#include <modm/architecture/interface/fiber/semaphore.hpp>
#include <modm/architecture/interface/fiber/latch.hpp>
#include <modm/architecture/interface/fiber/condition_variable.hpp>
#include <atomic>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>

namespace
{

constexpr uint16_t Iterations{1000};
constexpr uint8_t Contexts{6};
modm::fiber::Stack<> stack3, stack4;

/// Calls function(context) in two schedulers with two fibers each and in two
/// threads without a fiber. The test macros are not thread-safe, therefore
/// the function must only count errors.
void
run_parallel(auto &&function)
{
	std::thread scheduler0([&]
	{
		modm::fiber::Task fiber0(stack1, [&]{ function(0); });
		modm::fiber::Task fiber1(stack2, [&]{ function(1); });
		modm::fiber::Scheduler::run();
	});
	std::thread scheduler1([&]
	{
		modm::fiber::Task fiber2(stack3, [&]{ function(2); });
		modm::fiber::Task fiber3(stack4, [&]{ function(3); });
		modm::fiber::Scheduler::run();
	});
	std::thread thread4([&]{ function(4); });
	std::thread thread5([&]{ function(5); });
	scheduler0.join();
	scheduler1.join();
	thread4.join();
	thread5.join();
}

/// Runs a critical section that is protected by the lock functions.
void
test_exclusion(auto &&lock, auto &&unlock)
{
	std::atomic<uint8_t> inside{};
	std::atomic<uint16_t> errors{};
	uint32_t counter{}; // deliberately not atomic, is checked by TSan
	run_parallel([&](uint8_t)
	{
		for (uint16_t ii = 0; ii < Iterations; ii++)
		{
			lock();
			if (inside.fetch_add(1) != 0) errors++;
			counter = counter + 1;
			if (ii % 16 == 0) modm::this_fiber::yield();
			inside.fetch_sub(1);
			unlock();
			if (ii % 3 == 0) modm::this_fiber::yield();
		}
	});
	TEST_ASSERT_EQUALS(errors.load(), 0u);
	TEST_ASSERT_EQUALS(counter, uint32_t(Iterations) * Contexts);
}

struct TlsDestructor
{
	static inline uint8_t destroyed{};
	uint8_t used{};
	~TlsDestructor()
	{
		// The fiber has already ended, so this must not switch to another one
		modm::this_fiber::yield();
		destroyed++;
	}
};

}

void
FiberThreadTest::testThreadLocalDestructor()
{
	TlsDestructor::destroyed = 0;
	modm::fiber::Task fiber1(stack1, []
	{
		static thread_local TlsDestructor object;
		object.used = 1;
		modm::this_fiber::yield();
	});
	modm::fiber::Task fiber2(stack2, [&]
	{
		TEST_ASSERT_EQUALS(TlsDestructor::destroyed, 0u);
		fiber1.join();
		// The next fiber only continues after the destructors returned
		TEST_ASSERT_EQUALS(TlsDestructor::destroyed, 1u);
	});
	modm::fiber::Scheduler::run();
	TEST_ASSERT_EQUALS(TlsDestructor::destroyed, 1u);
}

void
FiberThreadTest::testYieldInsideCatch()
{
	const auto function = [](int value)
	{
		try { throw std::runtime_error(std::to_string(value)); }
		catch (const std::exception&)
		{
			for (uint8_t ii = 0; ii < 10; ii++)
			{
				modm::this_fiber::yield();
				try { throw; }
				catch (const std::exception& e) { TEST_ASSERT_EQUALS(std::stoi(e.what()), value); }
			}
		}
		TEST_ASSERT_FALSE(bool(std::current_exception()));
	};
	modm::fiber::Task fiber1(stack1, function, 1), fiber2(stack2, function, 2);
	modm::fiber::Scheduler::run();
}

void
FiberThreadTest::testSchedulersRunInParallel()
{
	// Every context spins without yielding until all others have started,
	// which deadlocks if any of them blocks another scheduler or thread.
	std::atomic<uint8_t> started{};
	std::atomic<uint8_t> finished{};
	run_parallel([&](uint8_t context)
	{
		// only the first fiber of each scheduler spins
		if (context != 1 and context != 3)
		{
			started++;
			while (started.load() < Contexts - 2) ;
		}
		modm::this_fiber::yield();
		finished++;
	});
	TEST_ASSERT_EQUALS(finished.load(), Contexts);
}

void
FiberThreadTest::testMutexAcrossSchedulers()
{
	modm::fiber::mutex mutex;
	test_exclusion([&]{ mutex.lock(); }, [&]{ mutex.unlock(); });
}

void
FiberThreadTest::testSharedMutexAcrossSchedulers()
{
	modm::fiber::shared_mutex mutex;
	test_exclusion([&]{ mutex.lock(); }, [&]{ mutex.unlock(); });

	std::atomic<int16_t> state{}; // writers are negative
	std::atomic<uint16_t> errors{};
	run_parallel([&](uint8_t context)
	{
		for (uint16_t ii = 0; ii < Iterations; ii++)
		{
			if ((ii + context) % 4 == 0)
			{
				mutex.lock();
				if (state.exchange(-1) != 0) errors++;
				modm::this_fiber::yield();
				state.store(0);
				mutex.unlock();
			}
			else
			{
				mutex.lock_shared();
				if (state.fetch_add(1) < 0) errors++;
				modm::this_fiber::yield();
				state.fetch_sub(1);
				mutex.unlock_shared();
			}
		}
	});
	TEST_ASSERT_EQUALS(errors.load(), 0u);
}

void
FiberThreadTest::testSemaphoreAcrossSchedulers()
{
	modm::fiber::binary_semaphore semaphore{1};
	test_exclusion([&]{ semaphore.acquire(); }, [&]{ semaphore.release(); });
}

void
FiberThreadTest::testConditionVariableAcrossSchedulers()
{
	modm::fiber::mutex mutex;
	modm::fiber::condition_variable cv;
	uint32_t items{}, produced{}, consumed{};
	run_parallel([&](uint8_t context)
	{
		for (uint16_t ii = 0; ii < Iterations; ii++)
		{
			mutex.lock();
			if (context % 2)
			{
				items++;
				produced++;
				mutex.unlock();
				cv.notify_all();
			}
			else
			{
				cv.wait(mutex, [&]{ return items > 0; });
				items--;
				consumed++;
				mutex.unlock();
			}
			modm::this_fiber::yield();
		}
	});
	TEST_ASSERT_EQUALS(items, 0u);
	TEST_ASSERT_EQUALS(produced, uint32_t(Iterations) * Contexts / 2);
	TEST_ASSERT_EQUALS(consumed, produced);
}

void
FiberThreadTest::testLatchAcrossSchedulers()
{
	modm::fiber::latch latch{Contexts};
	std::atomic<uint8_t> arrived{}, errors{};
	run_parallel([&](uint8_t)
	{
		arrived++;
		latch.arrive_and_wait();
		if (arrived.load() != Contexts) errors++;
	});
	TEST_ASSERT_EQUALS(errors.load(), 0u);
}

void
FiberThreadTest::testCallOnceAcrossSchedulers()
{
	modm::fiber::once_flag flag;
	uint8_t calls{};
	std::atomic<uint8_t> errors{};
	run_parallel([&](uint8_t)
	{
		modm::fiber::call_once(flag, [&]
		{
			modm::this_fiber::yield();
			calls = calls + 1;
		});
		// all callers wait for the active call
		if (calls != 1) errors++;
	});
	TEST_ASSERT_EQUALS(calls, 1u);
	TEST_ASSERT_EQUALS(errors.load(), 0u);
}

void
FiberThreadTest::testStopTokenAcrossSchedulers()
{
	modm::fiber::stop_state stop;
	std::atomic<uint8_t> waiting{}, stopped{};
	run_parallel([&](uint8_t context)
	{
		if (context == 5)
		{
			while (waiting.load() < Contexts - 1) modm::this_fiber::yield();
			stop.request_stop();
			return;
		}
		const auto token = stop.get_token();
		waiting++;
		while (not token.stop_requested()) modm::this_fiber::yield();
		stopped++;
	});
	TEST_ASSERT_EQUALS(stopped.load(), Contexts - 1);
}

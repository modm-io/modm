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

// Stresses fiber::recursive_mutex and fiber::barrier with randomized yields.
// On the RP2040 the fibers run on the schedulers of both cores. The mutex must
// keep a shared read-modify-write exclusive and stay recursive. The barrier
// must release all fibers exactly once per phase, after the completion.

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/architecture/interface/fiber/barrier.hpp>
#include <modm/architecture/interface/fiber/mutex.hpp>
#include <atomic>

#if __has_include(<modm/platform/core/multicore.hpp>)
#include <modm/platform/core/multicore.hpp>
#include <hardware/structs/rosc.h>
#define TEST_MULTICORE
static constexpr uint8_t Cores{2};
#else
static constexpr uint8_t Cores{1};
#endif

#include "../integration_test.hpp"

using namespace std::chrono_literals;

static constexpr auto Duration = 5s;
static constexpr uint8_t FibersPerGroup = 2 * Cores;
static constexpr uint8_t NoHolder{0xff};
#ifdef MODM_CPU_AVR
static constexpr size_t StackSize{256};
#else
static constexpr size_t StackSize{512};
#endif

static modm::Clock::time_point deadline;
static uint32_t seed{0x9E37'79B9};
static std::atomic<uint16_t> errors{0};

static bool
expired()
{
	return modm::Clock::now() >= deadline;
}

static uint32_t
next(uint32_t x)
{
	x ^= x << 13; x ^= x >> 17; x ^= x << 5;
	return x;
}

static void
yields(uint8_t count)
{
	while (count--) modm::this_fiber::yield();
}

// ============================== recursive_mutex =============================
static modm::fiber::recursive_mutex mutex;
static std::atomic<uint8_t> holder{NoHolder};
static volatile uint32_t mutex_shared{0};
static std::atomic<uint32_t> mutex_total{0};

static void
mutex_fiber(uint8_t id)
{
	uint32_t random = next(seed + id * 0x1000'0193ul);
	uint32_t local{0};
	while (not expired())
	{
		random = next(random);
		if (random & 1) mutex.lock();
		else while (not mutex.try_lock()) modm::this_fiber::yield();

		const uint8_t depth = (random >> 1) & 3;
		for (uint8_t ii = 0; ii < depth; ii++)
			if (not mutex.try_lock()) errors.fetch_add(1);

		if (holder.exchange(id, std::memory_order_relaxed) != NoHolder) errors.fetch_add(1);
		const uint32_t value = mutex_shared;
		yields((random >> 3) & 3);
		if (holder.load(std::memory_order_relaxed) != id) errors.fetch_add(1);
		mutex_shared = value + 1;
		holder.store(NoHolder, std::memory_order_relaxed);

		for (uint8_t ii = 0; ii < depth; ii++) mutex.unlock();
		mutex.unlock();
		local++;
		yields((random >> 5) & 1);
	}
	mutex_total.fetch_add(local);
}

// ================================== barrier =================================
static uint32_t slots[FibersPerGroup]{};
static uint32_t phase{0};
static bool stop{false};

struct Completion
{
	void
	operator()() const noexcept
	{
		for (const uint32_t slot : slots)
			if (slot != phase) errors.fetch_add(1);
		phase = phase + 1;
		stop = expired();
	}
};
static modm::fiber::barrier<Completion> sync_point{FibersPerGroup};

static void
barrier_fiber(uint8_t id)
{
	uint32_t random = next(~seed + id * 0x0100'0193ul);
	for (uint32_t round = 0; ; round++)
	{
		random = next(random);
		yields(random & 3);
		slots[id] = round;
		if (random & 4) sync_point.arrive_and_wait();
		else
		{
			const auto token = sync_point.arrive();
			yields((random >> 3) & 1);
			sync_point.wait(token);
		}
		if (phase != round + 1) errors.fetch_add(1);
		if (stop) break;
	}
}

// ============================================================================
static modm::Fiber<StackSize> mutex_fibers[FibersPerGroup];
static modm::Fiber<StackSize> barrier_fibers[FibersPerGroup];

static void
start_fibers(uint8_t core)
{
	for (uint8_t ii = 0; ii < 2; ii++)
	{
		const uint8_t id = core * 2 + ii;
		(void) mutex_fibers[id].start([id]{ mutex_fiber(id); });
		(void) barrier_fibers[id].start([id]{ barrier_fiber(id); });
	}
}

#ifdef TEST_MULTICORE
static std::atomic<bool> core1_done{false};

static void
core1_main()
{
	start_fibers(1);
	modm::fiber::Scheduler::run();
	core1_done = true;
	while (true) __WFE();
}
#endif

int
main()
{
	Board::initialize();
#ifdef TEST_MULTICORE
	// A different interleaving on every boot, the clocks are otherwise deterministic
	for (uint8_t ii = 0; ii < 32; ii++) seed = (seed << 1) | rosc_hw->randombit;
	seed |= 1;
#endif
	MODM_LOG_INFO << "fibers on cores: " << uint16_t(Cores) << modm::endl << modm::flush;

	deadline = modm::Clock::now() + Duration;
	start_fibers(0);
#ifdef TEST_MULTICORE
	modm::platform::multicore::Core1::run(core1_main);
#endif
	modm::fiber::Scheduler::run();
#ifdef TEST_MULTICORE
	while (not core1_done) ;
#endif

	const uint32_t shared = mutex_shared, total = mutex_total.load(), phases = phase;
	const uint16_t failures = errors.load();
	MODM_LOG_INFO << "mutex " << shared << " of " << total << " barrier phases " << phases
				  << " errors " << failures << modm::endl;
	return finishTest(failures == 0 and shared == total and total > 0 and phases > 0);
}

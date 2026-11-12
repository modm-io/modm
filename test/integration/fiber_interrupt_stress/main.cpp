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

// Synchronizes fibers with a timer interrupt that fires at pseudo-random
// intervals of 5-50us, and checks that no permit, notification or mutual
// exclusion gets lost.

#include <modm/board.hpp>
#include <modm/processing/fiber.hpp>
#include <modm/architecture/interface/fiber/condition_variable.hpp>
#include <modm/architecture/interface/fiber/latch.hpp>
#include <modm/architecture/interface/fiber/mutex.hpp>
#include <modm/architecture/interface/fiber/semaphore.hpp>
#include <modm/architecture/interface/fiber/shared_mutex.hpp>
#include <atomic>

#include "../integration_test.hpp"

using namespace std::chrono_literals;
using Timer = modm::platform::Timer2;

enum class
Phase : uint8_t
{
	Idle,
	Semaphore,
	BinarySemaphore,
	ConditionVariable,
	SharedMutex,
	Latch,
};
static std::atomic<Phase> phase{Phase::Idle};

static uint32_t
next_random(uint32_t &state)
{
	// xorshift32
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

static modm::fiber::counting_semaphore<1000> semaphore{0};
static std::atomic<uint32_t> released{0};
static modm::fiber::binary_semaphore signal{0};
static modm::fiber::condition_variable cv;
static std::atomic<uint32_t> produced{0};
static modm::fiber::shared_mutex shared;
static volatile uint8_t readers{0}, writers{0};
static std::atomic<uint32_t> isr_locks{0}, isr_reads{0}, violations{0};
static modm::fiber::latch latch{10'000};
static std::atomic<uint32_t> counted_down{0};

MODM_ISR(TIM2)
{
	static uint32_t random{1};
	Timer::acknowledgeInterruptFlags(Timer::InterruptFlag::Update);
	Timer::setOverflow(5 + next_random(random) % 46);
	switch (phase.load(std::memory_order_relaxed))
	{
		case Phase::Semaphore:
			released++;
			semaphore.release();
			break;
		case Phase::BinarySemaphore:
			signal.release();
			break;
		case Phase::ConditionVariable:
			produced++;
			cv.notify_one();
			break;
		case Phase::SharedMutex:
			if (shared.try_lock())
			{
				if (readers or writers) violations++;
				isr_locks++;
				shared.unlock();
			}
			if (shared.try_lock_shared())
			{
				if (writers) violations++;
				isr_reads++;
				shared.unlock_shared();
			}
			break;
		case Phase::Latch:
			if (not latch.try_wait())
			{
				counted_down++;
				latch.count_down();
			}
			break;
		default:
			break;
	}
}

static modm::fiber::Stack<> stack1, stack2;
static uint32_t fiber_random{7};

static void
random_yield()
{
	for (uint32_t count = next_random(fiber_random) % 4; count; count--)
		modm::this_fiber::yield();
}

static bool
running(modm::chrono::milli_clock::time_point deadline)
{
	return modm::chrono::milli_clock::now() < deadline;
}

static bool
check(const char* name, bool passed)
{
	MODM_LOG_INFO << (passed ? "pass: " : "FAIL: ") << name << modm::endl;
	return passed;
}

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "Fibers with interrupts at random intervals" << modm::endl;
	bool passed = true;

	Timer::enable();
	Timer::setMode(Timer::Mode::UpCounter);
	Timer::setPrescaler(Board::SystemClock::Timer2 / 1'000'000);
	Timer::setOverflow(20);
	Timer::applyAndReset();
	Timer::enableInterrupt(Timer::Interrupt::Update);
	Timer::enableInterruptVector(true, 10);
	Timer::start();

	// Every release of the interrupt must be acquired exactly once
	{
		uint32_t acquired[2]{};
		const auto deadline = modm::chrono::milli_clock::now() + 300ms;
		phase = Phase::Semaphore;
		auto consume = [&](uint8_t fiber)
		{
			while (running(deadline))
			{
				if (semaphore.try_acquire()) acquired[fiber]++;
				random_yield();
			}
		};
		modm::fiber::Task fiber1(stack1, consume, 0), fiber2(stack2, consume, 1);
		modm::fiber::Scheduler::run();
		phase = Phase::Idle;
		uint32_t total = acquired[0] + acquired[1];
		while (semaphore.try_acquire()) total++;
		MODM_LOG_INFO << "released " << released << ", acquired " << total << modm::endl;
		passed &= check("semaphore", released > 1000 and total == released);
	}

	// Many releases of a binary semaphore without acquire leave one permit
	{
		phase = Phase::BinarySemaphore;
		modm::this_fiber::sleep_for(10ms);
		phase = Phase::Idle;
		const bool first = signal.try_acquire();
		const bool second = signal.try_acquire();
		passed &= check("binary_semaphore", first and not second);
	}

	// The waiting fiber reacts within ~2us, the interrupt fires every 5-50us,
	// so a wait must end with the first notification, never with the second.
	// A random delay before every wait moves the start of the wait to random
	// points between two interrupts.
	{
		modm::fiber::mutex mutex;
		uint32_t consumed{produced}, wakes{0}, late{0};
		const auto deadline = modm::chrono::milli_clock::now() + 300ms;
		phase = Phase::ConditionVariable;
		modm::fiber::Task fiber1(stack1, [&]
		{
			mutex.lock();
			while (running(deadline))
			{
				modm::delay(std::chrono::microseconds(next_random(fiber_random) % 50));
				const uint32_t before = produced;
				if (not cv.wait_for(mutex, 1ms, [&]{ return produced != consumed; })) continue;
				const uint32_t now = produced;
				if (now - before > 1) late++;
				consumed = now;
				wakes++;
			}
			mutex.unlock();
		});
		modm::fiber::Scheduler::run();
		phase = Phase::Idle;
		MODM_LOG_INFO << "woken " << wakes << " times, " << late << " late" << modm::endl;
		passed &= check("condition_variable", wakes > 1000 and late == 0);
	}

	// The interrupt must never get the exclusive lock while fibers hold it,
	// and the reader count must be balanced afterwards
	{
		bool fiber_violation{false};
		const auto deadline = modm::chrono::milli_clock::now() + 300ms;
		phase = Phase::SharedMutex;
		auto use = [&]
		{
			while (running(deadline))
			{
				if (next_random(fiber_random) & 1)
				{
					shared.lock_shared();
					readers = readers + 1;
					random_yield();
					readers = readers - 1;
					shared.unlock_shared();
				}
				else
				{
					shared.lock();
					writers = writers + 1;
					if (readers or writers > 1) fiber_violation = true;
					random_yield();
					writers = writers - 1;
					shared.unlock();
				}
				random_yield();
			}
		};
		modm::fiber::Task fiber1(stack1, use), fiber2(stack2, use);
		modm::fiber::Scheduler::run();
		phase = Phase::Idle;
		const bool free = shared.try_lock();
		if (free) shared.unlock();
		MODM_LOG_INFO << "interrupt locked " << isr_locks << ", read " << isr_reads
					  << ", violations " << violations << modm::endl;
		passed &= check("shared_mutex", isr_locks > 100 and isr_reads > 100 and
						violations == 0 and not fiber_violation and free);
	}

	// Both fibers continue only after exactly all count downs
	{
		uint32_t seen[2]{};
		phase = Phase::Latch;
		auto wait = [&](uint8_t fiber) { latch.wait(); seen[fiber] = counted_down; };
		modm::fiber::Task fiber1(stack1, wait, 0), fiber2(stack2, wait, 1);
		modm::fiber::Scheduler::run();
		phase = Phase::Idle;
		passed &= check("latch", seen[0] == 10'000 and seen[1] == 10'000);
	}

	Timer::pause();
	return finishTest(passed);
}

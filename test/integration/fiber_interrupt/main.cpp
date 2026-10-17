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

// Synchronizes the main context with a timer interrupt via the fiber API
// without the fiber scheduler, where yield() is only an empty stub.

#include <modm/board.hpp>
#include <modm/architecture/interface/fiber.hpp>
#include <modm/architecture/interface/fiber/condition_variable.hpp>
#include <modm/architecture/interface/fiber/latch.hpp>
#include <modm/architecture/interface/fiber/mutex.hpp>
#include <modm/architecture/interface/fiber/semaphore.hpp>
#include <atomic>

#include "../integration_test.hpp"

using namespace std::chrono_literals;
using Timer = modm::platform::Timer2;

enum class
Phase : uint8_t
{
	Idle,
	Flag,
	Semaphore,
	Latch,
	Mutex,
	ConditionVariable,
};
static std::atomic<Phase> phase{Phase::Idle};

// Deliberately neither volatile nor atomic: yield() must force the reload
static bool flag{false};
static modm::fiber::binary_semaphore semaphore{0};
static modm::fiber::latch latch{100};
static modm::fiber::recursive_mutex mutex;
static uint32_t mutex_tries{0}, mutex_locked{0};
static modm::fiber::condition_variable cv;

MODM_ISR(TIM2)
{
	Timer::acknowledgeInterruptFlags(Timer::InterruptFlag::Update);
	switch (phase.load(std::memory_order_relaxed))
	{
		case Phase::Flag:
			flag = true;
			break;
		case Phase::Semaphore:
			semaphore.release();
			break;
		case Phase::Latch:
			latch.count_down();
			break;
		case Phase::Mutex:
			mutex_tries++;
			if (mutex.try_lock()) {
				mutex_locked++;
				mutex.unlock();
			}
			break;
		case Phase::ConditionVariable:
			cv.notify_one();
			break;
		default:
			break;
	}
}

static bool
check(const char* name, bool passed)
{
	MODM_LOG_INFO << (passed ? "pass: " : "FAIL: ") << name << modm::endl;
	return passed;
}

static uint32_t
elapsed_ms(modm::Clock::time_point start)
{
	return std::chrono::milliseconds(modm::Clock::now() - start).count();
}

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "Fiber API with interrupts and without scheduler" << modm::endl;
	bool passed = true;

	Timer::enable();
	Timer::setMode(Timer::Mode::UpCounter);
	Timer::setPeriod<Board::SystemClock>(1ms);
	Timer::applyAndReset();
	Timer::enableInterrupt(Timer::Interrupt::Update);
	Timer::enableInterruptVector(true, 10);
	Timer::start();

	// Outside of a fiber and without scheduler the ID must be zero
	passed &= check("get_id() == 0 in main", modm::this_fiber::get_id() == 0);

	// Without a memory barrier in yield() the load of the flag is hoisted out
	// of the polling loop and this times out
	phase = Phase::Flag;
	passed &= check("poll plain flag", modm::this_fiber::poll_for(100ms, []{ return flag; }));

	// Every interrupt releases the semaphore once, so this takes ~1s
	phase = Phase::Semaphore;
	auto start = modm::Clock::now();
	for (uint32_t ii = 0; ii < 1000; ii++) semaphore.acquire();
	uint32_t ms = elapsed_ms(start);
	MODM_LOG_INFO << "1000 acquires took " << ms << "ms" << modm::endl;
	passed &= check("semaphore", 980 <= ms and ms <= 1020);

	// Every interrupt counts down the latch once, so this takes ~100ms
	start = modm::Clock::now();
	phase = Phase::Latch;
	latch.wait();
	ms = elapsed_ms(start);
	MODM_LOG_INFO << "latch took " << ms << "ms" << modm::endl;
	passed &= check("latch", 95 <= ms and ms <= 105);

	// The interrupt must not be able to lock a mutex owned by main
	mutex.lock();
	phase = Phase::Mutex;
	modm::this_fiber::sleep_for(50ms);
	phase = Phase::Idle;
	mutex.unlock();
	MODM_LOG_INFO << "interrupt locked owned mutex " << mutex_locked << "/" << mutex_tries << modm::endl;
	passed &= check("recursive_mutex owned by main", mutex_tries >= 40 and mutex_locked == 0);

	// But it must be able to lock it when it is free
	mutex_tries = mutex_locked = 0;
	phase = Phase::Mutex;
	modm::this_fiber::sleep_for(10ms);
	phase = Phase::Idle;
	MODM_LOG_INFO << "interrupt locked free mutex " << mutex_locked << "/" << mutex_tries << modm::endl;
	passed &= check("recursive_mutex free", mutex_tries >= 5 and mutex_locked == mutex_tries);

	// The interrupt notifies every millisecond, so waiting must not time out
	modm::fiber::mutex cv_mutex;
	cv_mutex.lock();
	phase = Phase::ConditionVariable;
	passed &= check("condition_variable notified",
					cv.wait_for(cv_mutex, 10ms) == modm::fiber::cv_status::no_timeout);
	phase = Phase::Idle;
	passed &= check("condition_variable timeout",
					cv.wait_for(cv_mutex, 10ms) == modm::fiber::cv_status::timeout);
	cv_mutex.unlock();

	Timer::pause();
	return finishTest(passed);
}

/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing.hpp>
#include <modm/driver/time/cycle_counter.hpp>

using namespace Board;
using namespace std::chrono_literals;

constexpr uint32_t cycles = 100'000;
volatile uint32_t f1counter = 0, f2counter = 0;
uint32_t total_counter=0;
modm_fastdata modm::CycleCounter counter;

void
fiber_function1()
{
	MODM_LOG_INFO << MODM_FILE_INFO << modm::endl;
	while (++f1counter < cycles) { modm::this_fiber::yield(); total_counter++; }
}

void
fiber_function2(uint32_t cyc)
{
	MODM_LOG_INFO << MODM_FILE_INFO << modm::endl;
	while (++f2counter < cyc) { modm::this_fiber::yield(); total_counter++; }
}

struct Test
{
	void
	fiber_function3()
	{
		MODM_LOG_INFO << MODM_FILE_INFO << modm::endl;
		while (++f3counter < cycles) { modm::this_fiber::yield(); total_counter++; }
	}

	void
	fiber_function4(uint32_t cyc)
	{
		MODM_LOG_INFO << MODM_FILE_INFO << modm::endl;
		while (++f4counter < cyc) { modm::this_fiber::yield(); total_counter++; }
	}

	volatile uint32_t f3counter{0};
	volatile uint32_t f4counter{0};
} test;

// Single purpose fibers to time the yield
modm_faststack modm::Fiber<> fiber_y1([](){ modm::this_fiber::yield();  counter.stop(); });
modm_faststack modm::Fiber<> fiber_y2([](){ counter.start(); modm::this_fiber::yield(); });

modm_faststack modm::Fiber<> fiber1(fiber_function1, modm::fiber::Start::Later);
modm_faststack modm::Fiber<> fiber2([](){ fiber_function2(cycles); }, modm::fiber::Start::Later);
modm_faststack modm::Fiber<> fiber3([](){ test.fiber_function3(); }, modm::fiber::Start::Later);
modm_faststack modm::Fiber<> fiber4([cyc=uint32_t(0)]() mutable
{ cyc = cycles; test.fiber_function4(cyc); }, modm::fiber::Start::Later);

// Restartable Fibers
extern modm::Fiber<> fiber_pong;
extern modm::Fiber<> fiber_ping;
modm_faststack modm::Fiber<> fiber_ping([](){
	MODM_LOG_INFO << "ping = " << fiber_ping.stack_usage() << modm::endl;
	modm::this_fiber::sleep_for(1s);
	fiber_pong.start();
}, modm::fiber::Start::Later);
modm_faststack modm::Fiber<> fiber_pong([](){
	MODM_LOG_INFO << "pong = " << fiber_pong.stack_usage() << modm::endl;
	modm::this_fiber::sleep_for(1s);
	fiber_ping.start();
}, modm::fiber::Start::Later);

// Blue pill (M3 72MHz): Executed 1000000 in 1098591us (910256.88 yields per second)
// Feather M0 (M0+ 48MHz): Executed 1000000 in 1944692us (514220.25 yields per second)
int
main()
{
	Board::initialize();
	counter.initialize();
	MODM_LOG_INFO << "Starting fiber modm::yield benchmark..." << modm::endl;
	MODM_LOG_INFO.flush();

	fiber_y1.watermark_stack();
	fiber_y2.watermark_stack();
	// fiber_y1, fiber_y2 were autostarted
	{
		modm::atomic::Lock l;
		modm::fiber::Scheduler::run();
	}

	MODM_LOG_INFO << "Y1 stack usage: = " << fiber_y1.stack_usage() << modm::endl;
	MODM_LOG_INFO << "Y2 stack usage: = " << fiber_y2.stack_usage() << modm::endl;
	MODM_LOG_INFO.flush();

	// the rest is manually started
	fiber1.start(); fiber1.watermark_stack();
	fiber2.start(); fiber2.watermark_stack();
	fiber3.start(); fiber3.watermark_stack();
	fiber4.start(); fiber4.watermark_stack();
	const modm::PreciseTimestamp start = modm::PreciseClock::now();
	modm::fiber::Scheduler::run();
	const auto diff = (modm::PreciseClock::now() - start);

	MODM_LOG_INFO << "Benchmark done!" << modm::endl;
	MODM_LOG_INFO << "Executed " << total_counter << " yields in " << diff << modm::endl;
	MODM_LOG_INFO << ((total_counter * 1'000'000ull) / std::chrono::microseconds(diff).count());
	MODM_LOG_INFO << " yields per second, ";
	MODM_LOG_INFO << (std::chrono::nanoseconds(diff).count() / total_counter);
	MODM_LOG_INFO << "ns per yield slice" << modm::endl;
	MODM_LOG_INFO << counter.cycles() << " cycles = " << counter.nanoseconds();
	MODM_LOG_INFO << "ns per single yield" << modm::endl;

	MODM_LOG_INFO << "F1 stack usage = " << fiber1.stack_usage() << modm::endl;
	MODM_LOG_INFO << "F2 stack usage = " << fiber2.stack_usage() << modm::endl;
	MODM_LOG_INFO << "F3 stack usage = " << fiber3.stack_usage() << modm::endl;
	MODM_LOG_INFO << "F4 stack usage = " << fiber4.stack_usage() << modm::endl;

	fiber_ping.watermark_stack();
	fiber_pong.watermark_stack();
	fiber_ping.start();
	modm::fiber::Scheduler::run();

	while(1) ;
	return 0;
}

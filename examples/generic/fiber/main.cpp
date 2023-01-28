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

using namespace Board;
using namespace std::chrono_literals;

constexpr uint32_t cycles = 1'000'000;
volatile uint32_t f1counter = 0, f2counter = 0;
uint32_t total_counter=0;

void
fiber_function1()
{
	MODM_LOG_INFO << MODM_FILE_INFO << modm::endl;
	while (++f1counter < cycles) { modm::fiber::yield(); total_counter++; }
}

void
fiber_function2(uint32_t cyc)
{
	MODM_LOG_INFO << MODM_FILE_INFO << modm::endl;
	while (++f2counter < cyc) { modm::fiber::yield(); total_counter++; }
}

struct Test
{
	void
	fiber_function3()
	{
		MODM_LOG_INFO << MODM_FILE_INFO << modm::endl;
		while (++f3counter < cycles) { modm::fiber::yield(); total_counter++; }
	}

	void
	fiber_function4(uint32_t cyc)
	{
		MODM_LOG_INFO << MODM_FILE_INFO << modm::endl;
		while (++f4counter < cyc) { modm::fiber::yield(); total_counter++; }
	}

	volatile uint32_t f3counter{0};
	volatile uint32_t f4counter{0};
} test;

modm_faststack modm::fiber::Stack<2048> stack1;
modm_faststack modm::fiber::Stack<2048> stack2;
modm_faststack modm::fiber::Stack<2048> stack3;
modm_faststack modm::fiber::Stack<2048> stack4;
modm_fastdata  modm::Fiber fiber1(stack1, fiber_function1);
modm_fastdata  modm::Fiber fiber2(stack2, [](){ fiber_function2(cycles); });
modm_fastdata  modm::Fiber fiber3(stack3, [](){ test.fiber_function3(); });
modm_fastdata  modm::Fiber fiber4(stack4, [cyc=uint32_t(0)]() mutable { cyc++; test.fiber_function4(cyc); });

// Blue pill (M3 72MHz): Executed 1000000 in 1098591us (910256.88 yields per second)
// Feather M0 (M0+ 48MHz): Executed 1000000 in 1944692us (514220.25 yields per second)
int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "Starting fiber modm::yield benchmark..." << modm::endl;
	MODM_LOG_INFO.flush();

	const modm::PreciseTimestamp start = modm::PreciseClock::now();
	modm::fiber::Scheduler::run();
	const auto diff = (modm::PreciseClock::now() - start);

	MODM_LOG_INFO << "Benchmark done!" << modm::endl;
	MODM_LOG_INFO << "Executed " << total_counter << " yields in " << diff << modm::endl;
	MODM_LOG_INFO << ((total_counter * 1'000'000ull) / std::chrono::microseconds(diff).count());
	MODM_LOG_INFO << " yields per second, ";
	MODM_LOG_INFO << (std::chrono::nanoseconds(diff).count() / total_counter);
	MODM_LOG_INFO << "ns per yield" << modm::endl;
	MODM_LOG_INFO.flush();

	while(1) ;
	return 0;
}

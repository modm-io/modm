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

constexpr uint32_t cycles = 100'000;
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

modm::fiber::Stack<128> stack1;
modm::fiber::Stack<128> stack2;
modm::fiber::Stack<128> stack3;
modm::fiber::Stack<128> stack4;
modm::Fiber fiber1(stack1, fiber_function1);
modm::Fiber fiber2(stack2, [](){ fiber_function2(cycles); });
modm::Fiber fiber3(stack3, [](){ test.fiber_function3(); });
modm::Fiber fiber4(stack4, [cyc=uint32_t(cycles)]() mutable { cyc++; test.fiber_function4(cyc); });

// ATmega2560@16MHz: 239996 yields in 2492668us, 96280 yields per second, 10386ns per yield
int
main()
{
	Board::initialize();
	Board::LedD13::setOutput();
	MODM_LOG_INFO << "Starting fiber modm::yield benchmark..." << modm::endl;
	MODM_LOG_INFO.flush();

	const modm::PreciseTimestamp start = modm::PreciseClock::now();
	modm::fiber::Scheduler::run();
	const auto diff = (modm::PreciseClock::now() - start);

	MODM_LOG_INFO << "Benchmark done!" << modm::endl;
	MODM_LOG_INFO << "Executed " << total_counter << " yields in " << diff << modm::endl;
	MODM_LOG_INFO << uint32_t((total_counter * 1'000'000ull) / std::chrono::microseconds(diff).count());
	MODM_LOG_INFO << " yields per second, ";
	MODM_LOG_INFO << uint32_t(std::chrono::nanoseconds(diff).count() / total_counter);
	MODM_LOG_INFO << "ns per yield" << modm::endl;
	MODM_LOG_INFO.flush();

	while(1) ;
	return 0;
}

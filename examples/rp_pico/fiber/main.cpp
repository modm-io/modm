/*
 * Copyright (c) 2022, Andrey Kunitsyn
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
#include <mutex>

using namespace Board;
using namespace std::chrono_literals;

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper<Uart0, modm::IOBuffer::BlockIfFull> loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

constexpr uint32_t cycles = 1'000'000;

static multicore::Mutex log_mutex;

struct CoreData
{
	uint32_t total_counter = 0;
	uint32_t f1counter = 0;
	uint32_t f2counter = 0;
};

void
fiber_function1(CoreData& d)
{
	while (++d.f1counter < cycles)
	{
		modm::fiber::yield();
		d.total_counter++;
	}
}

void
fiber_function2(CoreData& d)
{
	while (++d.f2counter < cycles)
	{
		modm::fiber::yield();
		d.total_counter++;
	}
}

// put cores to mostly equivalent environment
modm_fastdata_core0 CoreData d0;
modm_fastdata_core1 CoreData d1;

modm_faststack_core0 modm::Fiber<256> fiber01([](){fiber_function1(d0);});
modm_faststack_core0 modm::Fiber<256> fiber02([](){fiber_function2(d0);});
// Do not autostart these fibers, otherwise they run on the Core0 scheduler!
modm_faststack_core1
modm::Fiber<256> fiber11([](){fiber_function1(d1);}, modm::fiber::Start::Later);
modm_faststack_core1
modm::Fiber<256> fiber12([](){fiber_function2(d1);}, modm::fiber::Start::Later);

template<typename TimeDiff>
static void
print_result(const CoreData& d, TimeDiff diff)
{
	std::lock_guard<multicore::Mutex> g(log_mutex);
	MODM_LOG_INFO << "Benchmark for core" << multicore::Core::cpuId() << " done!" << modm::endl;
	MODM_LOG_INFO << "Executed " << d.total_counter << " yields in " << diff << modm::endl;
	MODM_LOG_INFO << ((d.total_counter * 1'000'000ull) / std::chrono::microseconds(diff).count());
	MODM_LOG_INFO << " yields per second, ";
	MODM_LOG_INFO << (std::chrono::nanoseconds(diff).count() / d.total_counter);
	MODM_LOG_INFO << "ns per yield" << modm::endl;
	MODM_LOG_INFO.flush();
}

void
core1_main()
{
	// Start the fibers on the Core1 scheduler
	fiber11.start();
	fiber12.start();
	const modm::PreciseTimestamp start = modm::PreciseClock::now();
	modm::fiber::Scheduler::run();
	const auto diff = (modm::PreciseClock::now() - start);

	print_result(d1, diff);
	while (true) __NOP();
}

int
main()
{
	Board::initialize();

	// initialize Uart0 for MODM_LOG_*
	Uart0::connect<GpioOutput0::Tx>();
	Uart0::initialize<Board::SystemClock, 115200_Bd>();

	MODM_LOG_INFO << "Starting fiber modm::yield benchmark..." << modm::endl;
	MODM_LOG_INFO.flush();

	multicore::Core1::run(core1_main);

	const modm::PreciseTimestamp start = modm::PreciseClock::now();
	modm::fiber::Scheduler::run();
	const auto diff = (modm::PreciseClock::now() - start);

	print_result(d0, diff);
	while (true) __NOP();

	return 0;
}

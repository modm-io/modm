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

// ----------------------------------------------------------------------------
// Set the log level
#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper<Uart0, modm::IOBuffer::BlockIfFull> loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

//
#if 0
// with simple case we can use directly SpinLockUnsafe
using LockMutex = modm::platform::multicore::SpinLockUnsafe<1>;
#define LOG_GUARD()  modm::platform::multicore::SpinLockGuard<LockMutex> g
#define INIT_GUARD() LockMutex::init()
#else
// more extensive, but work at any case
static multicore::Mutex log_mutex;
#define LOG_GUARD() std::lock_guard<multicore::Mutex> g(log_mutex)
#define INIT_GUARD() \
	do {             \
	} while (false)
#endif

template<size_t Core, size_t Instance>
class Thread : public modm::Fiber<>
{
	static constexpr auto delay = 10ms + 1ms * Instance;

public:
	Thread() : Fiber([this]{ run(); }, Core == 0 ? modm::fiber::Start::Now : modm::fiber::Start::Later) {}

	void
	run()
	{
		uint32_t uptime{};
		while (true)
		{
			modm::this_fiber::sleep_for(delay);
			{
				// try without this line for intermixed output
				LOG_GUARD();
				MODM_LOG_INFO << "Core: " << multicore::Core::cpuId()
							  << " thread: " << Instance << " uptime: " << ++uptime << modm::endl;
			}
		}
	}
};

Thread<0, 0> t00;
Thread<0, 1> t01;
Thread<0, 2> t02;
Thread<0, 3> t03;

Thread<1, 0> t10;
Thread<1, 1> t11;
Thread<1, 2> t12;
Thread<1, 3> t13;

void
core1_main()
{
	t10.start();
	t11.start();
	t12.start();
	t13.start();
	modm::fiber::Scheduler::run();
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart0 for MODM_LOG_*
	Uart0::connect<GpioOutput0::Tx>();
	Uart0::initialize<Board::SystemClock, 115200_Bd>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG << "debug" << modm::endl;
	MODM_LOG_INFO << "info" << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR << "error" << modm::endl;

	INIT_GUARD();

	multicore::Core1::run(core1_main);
	modm::fiber::Scheduler::run();
	return 0;
}

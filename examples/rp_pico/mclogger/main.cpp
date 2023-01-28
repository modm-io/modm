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
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>
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
class Thread : public modm::pt::Protothread
{
	static constexpr auto delay = 10ms + 1ms * Instance;

public:
	Thread() { timeout.restart(delay); }

	bool
	update()
	{
		PT_BEGIN();
		while (true)
		{
			PT_WAIT_UNTIL(timeout.isExpired());
			timeout.restart(delay);
			{
				// try without this line for intermixed output
				LOG_GUARD();
				MODM_LOG_INFO << "Core: " << multicore::Core::cpuId()
							  << " thread: " << Instance << " uptime: " << ++uptime << modm::endl;
			}
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	uint32_t uptime;
};

template<size_t Core>
class Threads
{
private:
	Thread<Core, 0> t0;
	Thread<Core, 1> t1;
	Thread<Core, 2> t2;
	Thread<Core, 3> t3;

public:
	void
	update()
	{
		t0.update();
		t1.update();
		t2.update();
		t3.update();
	}
};

Threads<0> core0;
Threads<1> core1;

void
core1_main()
{
	while (true) { core1.update(); }
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

	while (true) { core0.update(); }

	return 0;
}

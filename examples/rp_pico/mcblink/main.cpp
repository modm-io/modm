/*
 * Copyright (c) 2023, Niklas Hauser
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

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper<Uart0, modm::IOBuffer::BlockIfFull> loggerDevice;
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

multicore::Mutex log_mutex;
uint32_t counter{};

void
core1_main()
{
	modm::PrecisePeriodicTimer tmr(0.100990s);
	uint32_t us_counter{};

	while (true)
	{
		const uint32_t us = modm::PreciseClock::now().time_since_epoch().count();
		if (us < us_counter)
		{
			std::lock_guard<multicore::Mutex> g(log_mutex);
			MODM_LOG_ERROR << us << " < " << us_counter << modm::endl;
		}
		us_counter = us;

		if (tmr.execute())
		{
			Board::LedGreen::set();
			std::lock_guard<multicore::Mutex> g(log_mutex);
			MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
		}
	}
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::LedGreen::setOutput();
	// initialize Uart0 for MODM_LOG_*
	Uart0::connect<GpioOutput0::Tx>();
	Uart0::initialize<Board::SystemClock, 115200_Bd>();

	multicore::Core1::run(core1_main);

	modm::PrecisePeriodicTimer tmr(0.1002284s);
	uint32_t us_counter{};

	while (true)
	{
		{
			const uint32_t us = modm::PreciseClock::now().time_since_epoch().count();
			if (us < us_counter)
			{
				std::lock_guard<multicore::Mutex> g(log_mutex);
				MODM_LOG_ERROR << us << " < " << us_counter << modm::endl;
			}
			us_counter = us;
		}

		if (tmr.execute())
		{
			Board::LedGreen::reset();
			std::lock_guard<multicore::Mutex> g(log_mutex);
			MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
		}
	}

	return 0;
}

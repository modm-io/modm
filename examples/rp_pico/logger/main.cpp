/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014, 2016, Sascha Schade
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

class BlinkThread : public modm::pt::Protothread
{
public:
	BlinkThread() { timeout.restart(100ms); }

	bool
	update()
	{
		PT_BEGIN();

		while (true)
		{
			Board::LedGreen::reset();

			PT_WAIT_UNTIL(timeout.isExpired());
			timeout.restart(100ms);

			Board::LedGreen::set();

			PT_WAIT_UNTIL(timeout.isExpired());
			timeout.restart(900ms);

			MODM_LOG_INFO << "Seconds since reboot: " << ++uptime << modm::endl;
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	uint32_t uptime;
};

BlinkThread blinkThread;

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

	while (true) { blinkThread.update(); }

	return 0;
}

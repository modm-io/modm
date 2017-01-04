/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013-2016, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2014, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);


class BlinkThread : public xpcc::pt::Protothread
{
public:
	BlinkThread()
	{
		timeout.restart(100);
	}

	bool
	update()
	{
		PT_BEGIN();

		while (true)
		{
			Board::LedGreen::reset();

			PT_WAIT_UNTIL(timeout.isExpired());
			timeout.restart(100);

			Board::LedGreen::set();

			PT_WAIT_UNTIL(timeout.isExpired()) ;
			timeout.restart(900);

			XPCC_LOG_INFO << "Seconds since reboot: " << ++uptime << xpcc::endl;
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	uint32_t uptime;
};

BlinkThread blinkThread;


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart2 for XPCC_LOG_*
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages
	XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << "error"   << xpcc::endl;


	while (true)
	{
		blinkThread.update();
	}

	return 0;
}

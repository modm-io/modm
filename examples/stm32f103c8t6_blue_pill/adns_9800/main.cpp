/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

#include <modm/driver/motion/adns9800.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

class BlinkThread : public modm::pt::Protothread
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
			timeout.restart(4900);

			MODM_LOG_INFO << "Seconds since reboot: " << uptime << modm::endl;

			uptime += 5;
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	uint32_t uptime;
};

class Adns9800Thread : public modm::pt::Protothread
{
public:
	Adns9800Thread() : timer(10), x(0), y(0)
	{
	}

	bool
	update()
	{
		PT_BEGIN();

		Cs::setOutput(modm::Gpio::High);

		SpiMaster1::connect<GpioOutputA7::Mosi, GpioOutputA5::Sck, GpioInputA6::Miso>();
		SpiMaster1::initialize<Board::systemClock, 2250000ul>();
		SpiMaster1::setDataMode(SpiMaster1::DataMode::Mode3);

		adns9800::initialise();

		while (true)
		{
			PT_WAIT_UNTIL(timer.execute());

			{
				int16_t delta_x, delta_y;
				adns9800::getDeltaXY(delta_x, delta_y);
				MODM_LOG_INFO.printf("dx = %5d, dy = %5d; x = %9d, y=%9d\n", delta_x, delta_y, x, y);

				x += delta_x;
				y += delta_y;
			}
		}

		PT_END();
	}

private:
	modm::ShortPeriodicTimer timer;
	int32_t x, y;

	using Cs = GpioOutputA4;

	using adns9800 = modm::Adns9800<
		/* Spi = */ SpiMaster1,
		/* Ncs = */ Cs >;
};


BlinkThread blinkThread;
Adns9800Thread adns9800Thread;


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart2 for MODM_LOG_*
	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	MODM_LOG_INFO << "Welcome to ADNS 9800 demo." << modm::endl;

	while (true)
	{
		blinkThread.update();
		adns9800Thread.update();
	}

	return 0;
}

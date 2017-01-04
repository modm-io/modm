/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
* Example to demonstrate a XPCC driver for Nokia 5110 displays.
*
* This example uses SpiMaster2 of STM32F407
*
* For the full pinout see the lcd namespace.
*
*/

#include <modm/debug/logger.hpp>
#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/driver/display/nokia5110.hpp>

#include <modm/architecture/platform.hpp>

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > device;
xpcc::IOStream stream(device);

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(device);
xpcc::log::Logger xpcc::log::info(device);
xpcc::log::Logger xpcc::log::warning(device);
xpcc::log::Logger xpcc::log::error(device);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

using namespace Board;

// Define the pins used by the LCD
namespace lcd
{
	using Reset     = GpioOutputD11;
	using Ce        = GpioOutputD10; // Chip select
	using Dc        = GpioOutputD9;  // Data / Command
	using Din       = GpioOutputB15; // Serial data in of display
	using Clk       = GpioOutputB13; // Clock
	using Backlight = GpioOutputE5;
}

// Select if hardware or software SPI Master shall be used
// typedef xpcc::SoftwareSpiMaster< lcd::Clk, lcd::Din> mySpiMaster;
typedef SpiMaster2 mySpiMaster;

// create a LCD object
xpcc::Nokia5110< mySpiMaster, lcd::Ce, lcd::Dc, lcd::Reset > display;

class ThreadOne : public xpcc::pt::Protothread
{
public:
	ThreadOne()
	{
	}

	bool
	update()
	{
		PT_BEGIN();

		lcd::Reset::setOutput(xpcc::Gpio::Low);
		lcd::Ce::setOutput(xpcc::Gpio::High);
		lcd::Dc::setOutput(xpcc::Gpio::Low);
		lcd::Backlight::setOutput(xpcc::Gpio::High);

		// Initialize
		display.initialize();

		display.setCursor(0, 0);

		// Write the standard welcome message ;-)
		display << "Hello xpcc.io";

		counter = 0;

		while (true)
		{
			display.setCursor(0, 10);
			display << counter << "   ";
			display.update();

			counter++;

			this->timeout.restart(1000);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	uint8_t counter;
};

ThreadOne one;

int
main()
{
	Board::initialize();

	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<Board::systemClock, xpcc::Uart::B115200>(10);

	XPCC_LOG_INFO << "\n\nWelcome to Nokia 5110 display demo!\n\n";

	// Software SPI Master
	// GpioOutputB15::setOutput();
	// GpioOutputB13::setOutput();

	// Hardware SPI Master
	GpioOutputB15::connect(SpiMaster2::Mosi);
	GpioOutputB13::connect(SpiMaster2::Sck);

	mySpiMaster::initialize<Board::systemClock, 2625000ul>();

	xpcc::ShortPeriodicTimer tmr(500);

	while(true)
	{
		one.update();
		if (tmr.execute()) {
			Board::LedOrange::toggle();
		}
	}
}

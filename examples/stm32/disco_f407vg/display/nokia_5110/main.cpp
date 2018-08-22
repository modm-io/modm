/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
* Example to demonstrate a MODM driver for Nokia 5110 displays.
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

#include <modm/board.hpp>

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > device;
modm::IOStream stream(device);

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(device);
modm::log::Logger modm::log::info(device);
modm::log::Logger modm::log::warning(device);
modm::log::Logger modm::log::error(device);

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

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
// typedef BitBangSpiMaster< lcd::Clk, lcd::Din> mySpiMaster;
typedef SpiMaster2 mySpiMaster;

// create a LCD object
modm::Nokia5110< mySpiMaster, lcd::Ce, lcd::Dc, lcd::Reset > display;

class ThreadOne : public modm::pt::Protothread
{
public:
	ThreadOne()
	{
	}

	bool
	update()
	{
		PT_BEGIN();

		lcd::Reset::setOutput(modm::Gpio::Low);
		lcd::Ce::setOutput(modm::Gpio::High);
		lcd::Dc::setOutput(modm::Gpio::Low);
		lcd::Backlight::setOutput(modm::Gpio::High);

		// Initialize
		display.initialize();

		display.setCursor(0, 0);

		// Write the standard welcome message ;-)
		display << "Hello modm.io";

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
	modm::ShortTimeout timeout;
	uint8_t counter;
};

ThreadOne one;

int
main()
{
	Board::initialize();

	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, modm::Uart::B115200>();

	MODM_LOG_INFO << "\n\nWelcome to Nokia 5110 display demo!\n\n";

	// Software SPI Master
	// GpioOutputB15::setOutput();
	// GpioOutputB13::setOutput();

	// Hardware SPI Master
	SpiMaster2::connect<GpioB15::Mosi, GpioB13::Sck>();
	mySpiMaster::initialize<Board::systemClock, 2625000ul>();

	modm::ShortPeriodicTimer tmr(500);

	while(true)
	{
		one.update();
		if (tmr.execute()) {
			Board::LedOrange::toggle();
		}
	}
}

/*
 * Copyright (c) 2015-2016, Sascha Schade
 * Copyright (c) 2015-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * Example to demonstrate a MODM driver for HD44780 displays,
 * including displays with I2C PCA8574 port expanders.
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA		PB11
 * SCL		PB10
 *
 * GND and +5V are connected to the the port expander of the display.
 *
 */

#include <modm/debug/logger.hpp>
#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/driver/display/hd44780.hpp>
#include <modm/driver/gpio/pca8574.hpp>

#include <modm/board/board.hpp>

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

typedef I2cMaster2 MyI2cMaster;
// typedef BitBangI2cMaster<GpioB10, GpioB11> MyI2cMaster;

// define the pins used by the LCD when not using a port expander
namespace lcd
{
	typedef GpioOutputC7 Backlight;
	typedef GpioOutputC6 E;
	typedef GpioOutputC5 Rw;
	typedef GpioOutputC4 Rs;
	// note: an 8bit data port
	typedef GpioPort<GpioOutputC0, 8> Data8Bit;
	// and a 4 bit data port
	typedef GpioPort<GpioOutputC4, 4> Data4Bit;
}

typedef modm::Pca8574<MyI2cMaster> GpioExpander;
GpioExpander gpioExpander;

namespace expander
{
	// Instances for each pin
	typedef GpioExpander::P0< gpioExpander > Rs;
	typedef GpioExpander::P1< gpioExpander > Rw;
	typedef GpioExpander::P2< gpioExpander > E;
	typedef GpioExpander::P3< gpioExpander > Backlight;
	typedef GpioExpander::P4< gpioExpander > Pin4;
	typedef GpioExpander::P5< gpioExpander > Pin5;
	typedef GpioExpander::P6< gpioExpander > Pin6;
	// you can also declare the pin like this, however it is too verbose
	typedef modm::GpioExpanderPin< GpioExpander, gpioExpander, GpioExpander::Pin::P7 > Pin7;
//	typedef GpioExpander::P7< gpioExpander > Pin7;

	// Form a GpioPort out of four single pins.
	typedef GpioExpander::Port< gpioExpander,
			GpioExpander::Pin::P4, 4 > Data4BitGpio;
	// You can also use SoftwareGpioPort, note however, that every pin is set separately,
	// so this requires four times as many bus accesses as the optimized version above.
//	typedef SoftwareGpioPort<Pin7, Pin6, Pin5, Pin4> Data4BitGpio;
}

// You can choose either port width simply by using it.
// The driver will handle it internally.

// create a LCD object with an 8bit data port
// modm::Hd44780< lcd::Data8Bit, lcd::Rw, lcd::Rs, lcd::E > display(20, 4);

// create a LCD object with an 4bit data port
// modm::Hd44780< lcd::Data4Bit, lcd::Rw, lcd::Rs, lcd::E  > display(20, 4);

// create a LCD object with an 4bit data port at a I2C Gpio port expander
modm::Hd44780< expander::Data4BitGpio, expander::Rw, expander::Rs, expander::E  > display(20, 4);

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
		MODM_LOG_DEBUG << "Ping the device from ThreadOne" << modm::endl;

		// ping the device until it responds
		while (true)
		{
			// we wait until the task started
			if (PT_CALL(gpioExpander.ping())) {
			 	break;
			}
			MODM_LOG_DEBUG << "Device did not respond" << modm::endl;

			// otherwise, try again in 100ms
			this->timeout.restart(1000);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		MODM_LOG_DEBUG << "Device responded" << modm::endl;

		// Actually, this is not needed because of hardware defaults, but this is better style.
		expander::Backlight::setOutput();
		expander::Data4BitGpio::setOutput();

		// Actually, this is not needed because of initialze of display driver.
		expander::Rs::setOutput();
		expander::Rw::setOutput();
		expander::E::setOutput();

		// Actually, this is not needed because of hardware defaults.
		expander::Backlight::set();

		// Initialize twice as some display are not initialised after first try.
		display.initialize();
		display.initialize();

		// Fill CGRAM
		display.writeCGRAM(0, cgA);
		display.writeCGRAM(1, cgB);

		display.setCursor(0, 0);

		// Write the standard welcome message ;-)
		display << "Hello modm.io **\n";

		// Write two special characters in second row
		display.setCursor(0, 1);
		display.write(0);
		display.write(1);

		counter = 0;

		while (true)
		{
			display.setCursor(3, 1);
			display << counter << "   ";

			counter++;

			this->timeout.restart(1000);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	uint8_t counter;

	// Bitmaps for special characters
	uint8_t cgA[8] = {0, 0b00100, 0b01110, 0b11111, 0b11111, 0b01110, 0b00100, 0};
	uint8_t cgB[8] = {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};
};

ThreadOne one;

int
main()
{
	Board::initialize();

	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, modm::Uart::B115200>();

	MODM_LOG_INFO << "\n\nWelcome to HD44780 I2C demo!\n\n";

	MyI2cMaster::connect<GpioB11::Sda, GpioB10::Scl>(MyI2cMaster::PullUps::Internal);
	MyI2cMaster::initialize<Board::systemClock, 100000>();

	modm::ShortPeriodicTimer tmr(500);

	while(true)
	{
		one.update();
		if (tmr.execute()) {
			Board::LedOrange::toggle();
		}
	}
}

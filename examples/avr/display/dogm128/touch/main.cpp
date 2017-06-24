/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2015, 2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/driver/display.hpp>
#include <modm/io/iostream.hpp>

using namespace modm::platform;
using systemClock = SystemClock;

namespace touch
{
	typedef GpioA0 Bottom;
	typedef GpioA1 Left;
	typedef GpioA2 Top;
	typedef GpioA3 Right;
}

namespace led
{
	typedef GpioOutputD7 R;
	typedef GpioOutputD6 G;
	typedef GpioOutputD5 B;
}

// define the pins used by the LCD
namespace lcd
{
	typedef GpioOutputB7 Scl;
	typedef GpioOutputB5 Mosi;

	typedef GpioOutputD2 Cs;
	typedef GpioOutputD3 A0;
	typedef GpioOutputD4 Reset;

	typedef BitBangSpiMaster< Scl, Mosi > SPI;
}

modm::DogM128< lcd::SPI, lcd::Cs, lcd::A0, lcd::Reset, true > display;

void
drawCross(uint8_t x, uint8_t y)
{
	display.drawPixel(x - 1, y - 1);
	display.drawPixel(x - 1, y + 1);
	display.drawPixel(x, y);
	display.drawPixel(x + 1, y - 1);
	display.drawPixel(x + 1, y + 1);
}

int
main()
{
	Uart0::connect<GpioOutputD1::Txd>();
	Uart0::initialize<systemClock, 115200>();

	// Enable interrupts, this is needed for every buffered UART
	enableInterrupts();

	// Create a IOStream for complex formatting tasks
	modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull > device;
	modm::IOStream output(device);

	output << "Welcome" << modm::endl;

	led::R::set();
	led::G::set();
	led::B::reset();

	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();

	lcd::SPI::connect<lcd::Scl::BitBang, lcd::Mosi::BitBang>();
	lcd::SPI::initialize<systemClock, MHz2>();

	display.initialize();

	// draw some testpoints
	drawCross( 10, 10);
	drawCross( 10, 54);
	drawCross(118, 10);
	drawCross(118, 54);
	drawCross( 62, 32);

	display.update();

	Adc::initialize<systemClock, 115000>();
	Adc::setReference(Adc::Reference::Internal2V56);

	touch::Bottom::setInput();
	touch::Bottom::reset();
	touch::Left::setInput();
	touch::Left::reset();
	touch::Top::setInput();
	touch::Top::reset();
	touch::Right::setInput();
	touch::Right::reset();

	while (1)
	{
		// read x value
		touch::Left::reset();
		touch::Left::setOutput();
		touch::Right::set();
		touch::Right::setOutput();
		modm::delayMilliseconds(2);

		int16_t xAdc = Adc::readChannel(0);

		touch::Left::setInput();
		touch::Right::setInput();
		touch::Right::reset();

		// read y value
		touch::Bottom::reset();
		touch::Bottom::setOutput();
		touch::Top::set();
		touch::Top::setOutput();
		modm::delayMilliseconds(2);

		int16_t yAdc = Adc::readChannel(1);

		touch::Bottom::setInput();
		touch::Top::setInput();
		touch::Top::reset();

		// print result
		output << "x=" << xAdc << ", y=" << yAdc << modm::endl;

		int16_t x = (static_cast<int32_t>(xAdc - 528) * (120 - 10)) / (115 - 528) + 10;
		int16_t y = (static_cast<int32_t>(yAdc - 236) * (58 - 10)) / (690 - 236) + 10;

		if (x < 0 || x > 128) {
			x = -1;
		}
		if (y < 0 || y > 64) {
			y = -1;
		}

		if (x >= 0 && y >= 0)
		{
			display.drawPixel(x, y);
			display.drawPixel(x, y+1);

			display.drawPixel(x+1, y);
			display.drawPixel(x+1, y+1);
		}

		display.update();
	}
}

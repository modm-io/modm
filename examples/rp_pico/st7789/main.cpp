/*
 * Copyright (c) 2022, Nikolay Semenov
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/display/st7789.hpp>
#include <modm/driver/display/st7789/st7789_spi_interface.hpp>
#include <modm/processing/timer/periodic_timer.hpp>
#include <modm/src/modm/ui/display/image/skull_64x64.hpp>

struct Display
{
	using Spi = SpiMaster0;
	using Cs = modm::platform::Gpio17;
	using Sck = modm::platform::Gpio18;
	using Miso = modm::platform::GpioUnused;
	using Mosi = modm::platform::Gpio19;
	using DataCommands = modm::platform::Gpio20;
	using Reset = modm::platform::Gpio21;
	using Backlight = modm::platform::GpioUnused;

	using Interface = modm::St7789SPIInterface<Spi, Cs, Reset, DataCommands>;
};

constexpr std::array<modm::color::Rgb565, 10> colors{
	modm::color::html::Black,  modm::color::html::Red,  modm::color::html::Green,
	modm::color::html::Blue,   modm::color::html::Cyan, modm::color::html::Magenta,
	modm::color::html::Yellow, modm::color::html::Gray, modm::color::html::Orange,
	modm::color::html::White,
};

uint32_t
rand(uint32_t limit)
{
	static uint32_t rand_next = 0;
	rand_next = rand_next * 1103515245 + 12345;
	return (uint32_t)(rand_next / 65536) % limit;
}

int
main()
{
	Board::initialize();

	Uart0::connect<GpioOutput1::Rx>();
	Uart0::initialize<Board::SystemClock, 115200_Bd>();

	modm::IODeviceWrapper<Uart0, modm::IOBuffer::BlockIfFull> terminal;

	Display::Spi::connect<Display::Sck::Sclk, Display::Mosi::Tx>();
	Display::Spi::initialize<Board::SystemClock, 62_MHz>();

	using LCD = modm::St7789<Display::Interface, 240, 240>;
	LCD lcd;

	lcd.initialize();

	lcd.clear();
	lcd.drawImage({88, 88}, modm::accessor::asFlash(bitmap::skull_64x64));

	while (true)
	{
		char c = 0;
		if (terminal.read(c))
		{
			if ('0' <= c && c <= '9')
			{
				lcd.clear(colors[c - '0'].color);
			} else if (c == 'f')
			{
				for (size_t i = 0; i < 1000; ++i)
				{
					lcd.setColor(colors[i % colors.size()]);
					lcd.drawCircle({rand(lcd.getWidth()), rand(lcd.getHeight())}, 16);
				}
			} else if (c == 'r')
			{
				static uint8_t orientation = 0;
				lcd.setOrientation((LCD::Driver::Orientation)(orientation++ % 4));
			} else if (c == 's' || c == 'S')
			{
				lcd.setSleep(c == 's');
			} else if (c == 'i' || c == 'I')
			{
				lcd.setIdle(c == 'i');
			} else if (c == 'o' || c == 'O')
			{
				c == 'o' ? lcd.turnOff() : lcd.turnOn();
			} else if (c == 'v' || c == 'V')
			{
				lcd.setInversion(c == 'V');
			} else if (c == 'b' || c == 'B')
			{
				lcd.setRgbBgrOrder(c == 'b' ? LCD::Driver::RgbBgrOrder::BGR
											: LCD::Driver::RgbBgrOrder::RGB);
			}
		}
	}

	return 0;
}

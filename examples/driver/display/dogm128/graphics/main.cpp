/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2018, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>
#include <modm/driver/display/ea_dog.hpp>
#include <modm/ui/display/font.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;
using namespace modm::literals;
using namespace std::chrono_literals;
using namespace modm::glcd;

// LCD Backlight
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

static void
drawShapes()
{
	display.clear();
	display.drawLine(Point(68, 50), Point(90, 50));
	display.drawLine(Point(78, 45), Point(78, 58));
	display.drawRoundedRectangle(Point(70, 13), 30, 20, 3);
	display.drawCircle(Point(105, 25), 20);
	display.drawEllipse(Point(60, 25), 20, 18);
	display.drawEllipse(Point(63, 31), 63, 31);
	display.fillRectangle(Point(10, 10), 20, 20);
	display.fillCircle(Point(45, 40), 23);
	display.fillRectangle(Point(20, 20), 20, 20);
	display.fillCircle(Point(55, 50), 8);
	display.drawLine(Point(0, 0), Point(127, 63));
	display.update();
}

static void
drawText()
{
	display.clear();
	display.setFont(modm::font::ScriptoNarrow);
	display.setCursor(Point(0, 0));
	display << "Hello World!\n";
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	display << "abcdefghijklmnopqrstuvwxyz\n";
	display << "0123456789!\"§$%&/()=?`´,;:-<>";

	display.setFont(modm::font::AllCaps3x5);
	display.setCursor(Point(0, 32));
	display << "Hello World!" << modm::endl;
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << modm::endl;
	display << "abcdefghijklmnopqrstuvwxyz" << modm::endl;
	display << 0 << 12 << 345 << 6789 << "!\"§$%&/()=?`´,;:-<>";
	display.update();
	modm::delay(2s);

	display.clear();
	display.setFont(modm::font::ArcadeClassic);
	display.setCursor(Point(0, 0));
	display << "Hello World!\n\n";
	display << "ABCDEFGHIJKLMNOP\nQRSTUVWXYZ\n";
	display << "0123456789!\"§$%&/\n()=?`´,;:-<>";
	display.update();
}

static void
drawCagedBall()
{
	int8_t xSpeed = 3;
	int8_t ySpeed = 1;
	uint8_t x = 15;
	uint8_t y = 15;
	const uint8_t radius = 5;

	for (uint16_t frame = 0; frame < 200; frame++)
	{
		display.clear();
		display.drawRectangle(Point(10, 10), 104, 44);

		x += xSpeed;
		y += ySpeed;

		if (x < 10 + radius + 3 || x > 114 - radius - 3) {
			xSpeed = -xSpeed;
		}
		if (y < 10 + radius + 1 || y > 54 - radius - 1) {
			ySpeed = -ySpeed;
		}

		display.drawCircle(Point(x, y), radius);
		display.update();

		modm::delay(50ms);
	}
}

int
main()
{
	// Enable a yellow backlight
	led::R::set();
	led::G::set();
	led::B::reset();

	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();

	lcd::SPI::connect<lcd::Scl::BitBang, lcd::Mosi::BitBang>();
	lcd::SPI::initialize<SystemClock, 2_MHz>();

	display.initialize();

	while (true)
	{
		drawShapes();
		modm::delay(2s);
		drawText();
		modm::delay(2s);
		drawCagedBall();
	}
}

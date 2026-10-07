/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/display/ili9341_spi.hpp>
using namespace Board;

namespace display
{
	using Spi = SpiMaster1;
	using Cs = modm::platform::GpioA4;
	using Sck = modm::platform::GpioA5;
	using Miso = modm::platform::GpioA6;
	using Mosi = modm::platform::GpioA7;
	using DataCommands = modm::platform::GpioB0;
	using Reset = modm::platform::GpioB1;
	using Backlight = modm::platform::GpioB10;
}

modm::Ili9341Spi<
	display::Spi,
	display::Cs,
	display::DataCommands,
	display::Reset,
	display::Backlight
> tft;

/*
 * Blinks the green user LED with 1 Hz.
 * It is on for 90% of the time and off for 10% of the time.
 */

int
main()
{
	using namespace modm::literals;
	Board::initialize();
	display::Spi::connect<
		display::Sck::Sck, display::Miso::Miso, display::Mosi::Mosi>();
	display::Spi::initialize<SystemClock, 1125_kHz>();
	tft.initialize();
	tft.enableBacklight(true);
	LedGreen::set();
	tft.setColor(modm::color::html::Black);

	int16_t w = tft.getWidth();
	int16_t h = tft.getHeight();
	tft.drawLine({0,0}, {w, h});
	tft.drawLine({w,0}, {0, h});

	while (true)
	{
		LedGreen::set();
		modm::delay(900ms);

		LedGreen::reset();
		modm::delay(100ms);
	}

	return 0;
}

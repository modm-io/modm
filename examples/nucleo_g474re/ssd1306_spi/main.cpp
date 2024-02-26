/*
 * Copyright (c) 2023, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/display/ssd1306_spi.hpp>

/**
 * SSD1306 display in SPI 4-wire mode
 *
 * SCK		<-> Arduino D3 / GpioB3
 * MOSI		<-> Arduino D4 / GpioB5
 * CS		<-> Arduino D5 / GpioB4
 * D/C#		<-> Arduino D2 / GpioA10
 * RESET#	<-> Arduino D7 / GpioA8
 *
 * En7V5	<-> Arduino D6 / GpioB10
 *
 * TODO: Describe charge pump setup for 7.5V
 */

using MySpiMaster = modm::platform::SpiMaster1;
using Sck = Board::D3;
using Mosi = Board::D4;
using Cs = Board::D5;
using Dc = Board::D2;
using Reset = Board::D7;
using En7V5 = Board::D6;
using Display = modm::Ssd1306Spi<MySpiMaster, Cs, Dc, 32>;
Display display;

int
main()
{
	Board::initialize();

	En7V5::reset();
	En7V5::setOutput();
	Reset::reset();
	Reset::setOutput();

	Cs::setOutput();
	Dc::setOutput();
	MySpiMaster::connect<Sck::Sck, Mosi::Mosi>();
	MySpiMaster::initialize<Board::SystemClock, 660_kHz>();

	Cs::set();
	En7V5::set();
	modm::delay(500ms);

	Reset::set();
	modm::delay(1ms);

	RF_CALL_BLOCKING(display.initialize());
	RF_CALL_BLOCKING(display.setOrientation(modm::glcd::Orientation::Landscape0));
	RF_CALL_BLOCKING(display.setDisplayMode(Display::DisplayMode::Inverted));
	RF_CALL_BLOCKING(display.setContrast(80));

	display.setFont(modm::font::Assertion);

	modm::ShortPeriodicTimer timer(333ms);
	uint16_t counter(0);

	while (true)
	{
		if (timer.execute())
		{
			display.clear();
			display.setCursor(1, 1);
			display << "Hello World!";
			display.setCursor(1,17);
			display << counter++;

			display.update();
		}
	}

	return 0;
}

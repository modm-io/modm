/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/pwm/apa102.hpp>
#include <modm/ui/led/tables.hpp>
#include <modm/processing/timer.hpp>

using namespace Board;

modm::Apa102<SpiMaster1, 8*8> leds;
modm::ShortPeriodicTimer tmr{33ms};

int
main()
{
	Board::initialize();
	SpiMaster1::connect<Board::D13::Sck, Board::D11::Mosi>();
	SpiMaster1::initialize<Board::SystemClock, 8_MHz>();

	constexpr uint8_t max = 62;
	uint8_t r=0, g=max/3, b=max/3*2;

	while (true)
	{
		for (size_t ii=0; ii < leds.size; ii++)
		{
			leds.setColor(ii, {r, g, b});
						  // {modm::ui::table22_8_256[r],
						  //  modm::ui::table22_8_256[g],
						  //  modm::ui::table22_8_256[b]});
			if (r++ >= max) r = 0;
			if (g++ >= max) g = 0;
			if (b++ >= max) b = 0;
		}
		RF_CALL_BLOCKING(leds.write());

		while(not tmr.execute()) ;
	}

	return 0;
}

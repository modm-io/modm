/*
 * Copyright (c) 2019, Niklas Hauser
 * Copyright (c) 2023, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/pwm/ws2812b.hpp>
#include <modm/ui/led/tables.hpp>
#include <modm/processing/timer.hpp>
#include <modm/processing.hpp>

using namespace Board;

using Output = Board::D11;
using DmaRx = Dma1::Channel3;
using DmaTx = Dma1::Channel4;
using SpiLed = SpiMaster2_Dma<DmaRx, DmaTx>;
// using SpiLed = SpiMaster2; // for non-dma version
modm::Ws2812b<SpiLed, Output, 8*8> leds;
modm::ShortPeriodicTimer tmr{33ms};

constexpr uint8_t max = 62;
uint8_t r=0, g=max/3, b=max/3*2;

int
main()
{
	Board::initialize();
	LedD13::setOutput();
	Dma1::enable();
	leds.initialize<Board::SystemClock>();

	constexpr uint8_t max = 60;
	uint8_t r=0, g=max/3, b=max/3*2;

	while (true)
	{
		for (size_t ii=0; ii < leds.size; ii++)
		{
			leds.setColor(ii,
						  {modm::ui::table22_8_256[r*3/2],
						   modm::ui::table22_8_256[g*3/2],
						   modm::ui::table22_8_256[b*3/2]});
			if (r++ >= max) r = 0;
			if (g++ >= max) g = 0;
			if (b++ >= max) b = 0;
		}
		leds.write();

		while(not tmr.execute()) ;
		LedD13::toggle();
	}

	return 0;
}

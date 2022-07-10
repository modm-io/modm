/*
 * Copyright (c) 2019, Niklas Hauser
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
using DmaRx = Dma2::Channel0;
using DmaTx = Dma2::Channel3;
using SpiLed = SpiMaster1_Dma<DmaRx, DmaTx>;
modm::Ws2812b<SpiLed, Output, 8*8> leds;
modm::ShortPeriodicTimer tmr{33ms};

constexpr uint8_t max = 62;
uint8_t r=0, g=max/3, b=max/3*2;

class Ws2812Thread : public modm::pt::Protothread
{
public:

	bool
	update()
	{
		PT_BEGIN();

		LedD13::setOutput();
		Dma2::enable();
		leds.initialize<Board::SystemClock>();

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
			PT_CALL(leds.write());

			LedD13::toggle();
			timeout.restart(500ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
};

Ws2812Thread ws2812_thread;

int
main()
{
	Board::initialize();



	while (true)
	{
		ws2812_thread.update();
	}

	return 0;
}

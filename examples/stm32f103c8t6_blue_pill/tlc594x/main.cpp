/*
 * Copyright (c) 2019, Linas Nikiperavicius
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/pwm/tlc594x.hpp>

using namespace Board;

/*
 * Turns on each TLC594* output in ascending order.
 * Each output is active for 500 milliseconds.
 */

#define CHANNELS 24

modm::TLC594X< CHANNELS, SpiMaster1, GpioA4, GpioA3 > tlc594x;

int
main()
{
	Board::initialize();

	SpiMaster1::connect< GpioA5::Sck, GpioA7::Mosi > ();
	SpiMaster1::initialize< Board::SystemClock, 1125_kBd>();

	tlc594x.initialize(0, -1, true, false, true);

	while (true)
	{
		for (uint16_t i = 0; i < CHANNELS; ++i)
		{
			tlc594x.setAllChannels(0x000);
			tlc594x.setChannel(i, 0xfff);
			RF_CALL_BLOCKING(tlc594x.writeChannels());

			modm::delay(500ms);
		}
	}

	return 0;
}

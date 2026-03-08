/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2013-2014, 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
# nRF5340-DK blink (app core)

Basic blink example for the nRF5340 application core.

- Target: `nrf5340-xxaa@app`
- Uses app-core-owned LEDs/buttons from the BSP.
- Validates startup, clock configuration and GPIO operation on the app core.
*/

#include <modm/board.hpp>

int
main()
{
	Board::initialize();

	uint32_t counter(0);

	while (true)
	{
		Board::Leds::write(1 << (counter % Board::Leds::width));
		modm::delay(Board::Buttons::read() ? 100ms : 500ms);
		counter++;
	}
}


/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
# nRF5340-DK UART logging (network core)

UART logger example for the nRF5340 network core.

- Target: `nrf5340-xxaa@net`
- Uses net-core logger routing configured by the BSP.
- Demonstrates logging and LED/button interaction on net-core-owned pins.

This example complements app-core examples and is useful to verify dual-core
BSP behavior.
*/

#include <modm/board.hpp>

int
main()
{
	Board::initialize();

	uint32_t counter(0);
	while (true)
	{
		Board::Led3::toggle();
		modm::delay(Board::Button3::read() ? 100ms : 500ms);
		counter++;
        MODM_LOG_INFO << "Counter: " << counter << modm::endl;
	}
}

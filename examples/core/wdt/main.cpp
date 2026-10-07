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
# nRF5340-DK watchdog (app core)

Watchdog example for the nRF5340 application core.

- Target: `nrf5340-xxaa@app`
- Uses app-core-owned `Led1` and `Button1` from the BSP.
- If `Button1` is held for more than 4 seconds after the watchdog starts,
  the MCU resets.
- A watchdog reset is indicated by a faster LED blink sequence after startup.
*/

#include <modm/board.hpp>

using namespace Board;
using namespace std::chrono_literals;

int
main()
{
	Board::initialize();
	Wdt::initialize<SystemClock, 4s>();
	auto startupDelay = 100ms;

	MODM_LOG_INFO << "nRF5340-DK watchdog example" << modm::endl;
	MODM_LOG_INFO << "Press and hold Button1 for more than 4s to trigger a watchdog reset." << modm::endl;
	if (NRF_RESET->RESETREAS & RESET_RESETREAS_DOG0_Msk) {
		NRF_RESET->RESETREAS = RESET_RESETREAS_DOG0_Msk;
		MODM_LOG_INFO << "Previous reset reason: watchdog." << modm::endl;
		startupDelay = 50ms;
	}

	uint32_t counter(0);
	while (counter < 10)
	{
		Led1::toggle();
		modm::delay(startupDelay);
		MODM_LOG_INFO << "startup: " << counter++ << modm::endl;
	}

	Wdt::enable();
	MODM_LOG_INFO << "Watchdog started." << modm::endl;

	while (true)
	{
		Led1::toggle();
		modm::delay(500ms);
		if (!Button1::read()) { Wdt::trigger(); }
		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	return 0;
}
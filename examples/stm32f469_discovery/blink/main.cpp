/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint32_t counter(0);
	modm::PrecisePeriodicTimer tmr(0.500990s);
	modm::PeriodicTimer tmrS(0.500990s);

	uint32_t ms_counter{0};
	uint32_t us_counter{0};

	while (true)
	{
		{
			uint32_t ms = modm::Clock::now().time_since_epoch().count();
			if (ms < ms_counter) {
				MODM_LOG_ERROR << ms << " < " << ms_counter << modm::endl;
			}
			ms_counter = ms;
		}{
			uint32_t us = modm::PreciseClock::now().time_since_epoch().count();
			if (us < us_counter) {
				MODM_LOG_ERROR << us << " < " << us_counter << modm::endl;
			}
			us_counter = us;
		}

		if (tmr.execute())
		{
			LedBlue::toggle();

			MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
		}

		if (tmrS.execute())
		{
			LedGreen::toggle();
		}
	}

	return 0;
}

/*
 * Copyright (c) 2020, Niklas Hauser
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

int main()
{
	Board::initialize();

	uint32_t counter(0);
	modm::PrecisePeriodicTimer ptmr(0.500990s);
	modm::PeriodicTimer tmr(0.500990s);

	for (int ii=0; ii<20; ii++)
	{
		LedD13::toggle();
		modm::delay(std::chrono::milliseconds(10*ii));
	}

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

		if (ptmr.execute())
		{
			LedD13::set();
			MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
		}
		if (tmr.execute())
		{
			LedD13::reset();
		}
	}
}

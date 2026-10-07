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

#include "../integration_test.hpp"

/**
 * Tests that the millisecond and the microsecond clock never run backwards,
 * agree with each other and that the software timers expire on time.
 */

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	bool passed = true;

	modm::PrecisePeriodicTimer preciseTimer(500ms);
	modm::PeriodicTimer timer(500ms);
	uint8_t preciseExpired{0}, expired{0};

	const uint32_t ms_start = modm::Clock::now().time_since_epoch().count();
	const uint32_t us_start = modm::PreciseClock::now().time_since_epoch().count();
	uint32_t ms_last{ms_start}, us_last{us_start};

	// run for a bit more than three seconds
	while (ms_last - ms_start < 3250)
	{
		const uint32_t ms = modm::Clock::now().time_since_epoch().count();
		if (ms < ms_last) {
			MODM_LOG_ERROR << ms << " < " << ms_last << modm::endl;
			passed = false;
		}
		ms_last = ms;

		const uint32_t us = modm::PreciseClock::now().time_since_epoch().count();
		if (us < us_last) {
			MODM_LOG_ERROR << us << " < " << us_last << modm::endl;
			passed = false;
		}
		us_last = us;

		if (preciseTimer.execute()) { preciseExpired++; Board::Leds::toggle(); }
		if (timer.execute()) expired++;
	}

	// both clocks must have measured the same time
	const int32_t difference = int32_t((us_last - us_start) / 1000) - int32_t(ms_last - ms_start);
	MODM_LOG_INFO << "Clocks differ by " << difference << "ms, timers expired " << expired
				  << " and " << preciseExpired << " times" << modm::endl;
	passed &= (-2 <= difference and difference <= 2);
	passed &= (expired == 6) and (preciseExpired == 6);

	return finishTest(passed);
}

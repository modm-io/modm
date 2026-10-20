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

#pragma once

#include <modm/architecture/interface/clock.hpp>
#include <modm/platform/device.hpp>
#include <hardware/structs/watchdog.h>

namespace modm::platform
{

/**
 * Provides `modm::Clock` and `modm::PreciseClock` from the 64-bit 1MHz TIMER,
 * which is shared by both cores.
 *
 * @ingroup modm_platform_clock
 */
class TimerClock
{
public:
	/// Starts the 1MHz watchdog tick from clk_ref, which the TIMER counts.
	template< class SystemClock >
	static void
	initialize()
	{
		static_assert(SystemClock::RefFrequency % 1'000'000 == 0,
					  "clk_ref must be an integer multiple of 1MHz for the TIMER tick!");
		watchdog_hw->tick = WATCHDOG_TICK_ENABLE_BITS | (SystemClock::RefFrequency / 1'000'000);
	}
};

}	// namespace modm::platform

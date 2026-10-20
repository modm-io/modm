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

#include "timer_clock.hpp"
#include <hardware/structs/timer.h>

// Allow the ability to override the clock now functions
#ifndef MODM_CHRONO_MILLI_CLOCK_NOW
#	define MODM_CHRONO_MILLI_CLOCK_NOW modm::chrono::milli_clock::now
#endif
#ifndef MODM_CHRONO_MICRO_CLOCK_NOW
#	define MODM_CHRONO_MICRO_CLOCK_NOW modm::chrono::micro_clock::now
#endif

// x / 1000 as mulhi(x, 0x10624DD3) >> 6 from 16-bit halves, since the M0+ has
// no 32x32=64-bit multiplication.
static modm_always_inline uint32_t
div1000(uint32_t x)
{
	const uint32_t xl = x & 0xffff, xh = x >> 16;
	constexpr uint32_t ml = 0x4DD3, mh = 0x1062;
	const uint32_t mid = ((xl * ml) >> 16) + xh * ml;
	const uint32_t mid2 = (mid & 0xffff) + xl * mh;
	return (xh * mh + (mid >> 16) + (mid2 >> 16)) >> 6;
}

modm::chrono::milli_clock::time_point modm_weak
MODM_CHRONO_MILLI_CLOCK_NOW() noexcept
{
	uint32_t hi, lo;
	do {
		hi = timer_hw->timerawh;
		lo = timer_hw->timerawl;
	} while (hi != timer_hw->timerawh);
	// Avoid the slow 64-bit division with 2^32 = 4294967 * 1000 + 296, which is
	// exact for less than 1976 years of uptime.
	const uint32_t lo_ms = div1000(lo);
	const uint32_t ms = hi * 4294967u + lo_ms + div1000(hi * 296u + (lo - lo_ms * 1000u));
	return modm::chrono::milli_clock::time_point{modm::chrono::milli_clock::duration{ms}};
}

modm::chrono::micro_clock::time_point modm_weak
MODM_CHRONO_MICRO_CLOCK_NOW() noexcept
{
	return modm::chrono::micro_clock::time_point{
			modm::chrono::micro_clock::duration{timer_hw->timerawl}};
}

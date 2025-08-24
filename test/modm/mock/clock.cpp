/*
 * Copyright (c) 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/interface/clock.hpp>
#include "clock.hpp"

// ----------------------------------------------------------------------------
static uint32_t milli_time{0};
static bool milli_enabled{false};

modm::chrono::milli_clock::time_point
modm_platform_milli_now() noexcept;

modm::chrono::milli_clock::time_point
modm::chrono::milli_clock::now() noexcept
{
	if (milli_enabled) return time_point{duration{milli_time}};
	return modm_platform_milli_now();
}

void
modm_test::chrono::milli_clock::setTime(uint32_t milliseconds)
{
	milli_time = milliseconds;
}

void
modm_test::chrono::milli_clock::increment(uint32_t milliseconds)
{
	milli_time += milliseconds;
}

void
modm_test::chrono::milli_clock::enable()
{
	milli_enabled = true;
}

void
modm_test::chrono::milli_clock::disable()
{
	milli_enabled = false;
}

// ----------------------------------------------------------------------------
static uint32_t micro_time{0};
static bool micro_enabled{false};

modm::chrono::micro_clock::time_point
modm_platform_micro_now() noexcept;

modm::chrono::micro_clock::time_point
modm::chrono::micro_clock::now() noexcept
{
	if (micro_enabled) return time_point{duration{micro_time}};
	return modm_platform_micro_now();
}
void
modm_test::chrono::micro_clock::setTime(uint32_t microseconds)
{
	micro_time = microseconds;
}

void
modm_test::chrono::micro_clock::increment(uint32_t microseconds)
{
	micro_time += microseconds;
}

void
modm_test::chrono::micro_clock::enable()
{
	micro_enabled = true;
}

void
modm_test::chrono::micro_clock::disable()
{
	micro_enabled = false;
}

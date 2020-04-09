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

modm::chrono::milli_clock::time_point
modm::chrono::milli_clock::now() noexcept
{
	return time_point{duration{milli_time}};
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

// ----------------------------------------------------------------------------
static uint32_t micro_time{0};

modm::chrono::micro_clock::time_point
modm::chrono::micro_clock::now() noexcept
{
	return time_point{duration{micro_time}};
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

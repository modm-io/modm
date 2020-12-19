/*
 * Copyright (c) 2019, Benjamin Weps
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/interface/clock.hpp>

#define RTC_COUNT_LO *((volatile uint32_t *)0x10000048)
#define RTC_COUNT_HI *((volatile uint32_t *)0x1000004C)


modm::chrono::milli_clock::time_point modm_weak
modm::chrono::milli_clock::now() noexcept
{
    uint64_t ctrVal = RTC_COUNT_HI;
    ctrVal = ctrVal << 32;
    ctrVal |= RTC_COUNT_LO;
	return time_point{duration{static_cast<uint32_t>(ctrVal/33)}};
}

modm::chrono::micro_clock::time_point modm_weak
modm::chrono::micro_clock::now() noexcept
{
	uint32_t time = 0;
	return time_point{duration{time}};
}

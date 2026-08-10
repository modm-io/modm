/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/architecture/interface/clock.hpp>

#include <lwip/arch.h>

extern "C" u32_t
sys_now(void)
{
	return static_cast<u32_t>(modm::Clock::now().time_since_epoch().count());
}

/*
 * Copyright (c) 2021 Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "lv_modm_clock.h"
#include <modm/architecture/interface/clock.hpp>

uint32_t lv_modm_clock_now()
{
    return modm::Clock::now().time_since_epoch().count();
}

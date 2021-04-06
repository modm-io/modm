/*
 * Copyright (c) 2021 Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LV_MODM_CLOCK_HPP
#define LV_MODM_CLOCK_HPP

#include <stdint.h>
#include <modm/architecture/utils.hpp>

modm_extern_c uint32_t
lv_modm_clock_now(void);

#endif

/*
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_COMMON_CLOCK_HPP
#define MODM_COMMON_CLOCK_HPP

#include <stdint.h>
#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface/clock.hpp>

namespace modm::clock
{

/// @cond
extern uint32_t fcpu;
extern uint32_t fcpu_kHz;
extern uint16_t fcpu_MHz;
extern uint16_t ns_per_loop;
/// @endcond

}


#endif	// MODM_COMMON_CLOCK_HPP

/*
 * Copyright (c) 2012, 2014-2019, 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "rcc.hpp"

// CMSIS Core compliance
constinit uint32_t modm_fastdata SystemCoreClock(modm::platform::Rcc::BootFrequency);
modm_weak void SystemCoreClockUpdate() { /* Nothing to update */ }

namespace modm::platform
{
constinit uint16_t modm_fastdata delay_fcpu_MHz(computeDelayMhz(Rcc::BootFrequency));
constinit uint16_t modm_fastdata delay_ns_per_loop(computeDelayNsPerLoop(Rcc::BootFrequency));

}

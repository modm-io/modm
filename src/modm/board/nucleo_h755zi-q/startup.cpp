/*
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>

using namespace modm::platform;

#ifdef CORE_CM7

extern "C" void
modm_initialize_platform(void)
{
	// Configure switch mode power supply
	Rcc::configurePowerSource(Rcc::PowerSource::SmpsDirect);
}

#endif

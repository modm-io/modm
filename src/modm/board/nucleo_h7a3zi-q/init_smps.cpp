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

#include <modm/platform.hpp>

using namespace modm::platform;

// Called from the Reset_Handler before any RAM is accessed, so this must only
// touch registers and must not use the stack, see modm:platform:core.
extern "C" void
modm_initialize_platform(void)
{
	// The board is wired for the direct SMPS supply (UM2408 supply config 2).
	// A different supply configuration locks up the device!
	Rcc::configurePowerSource(Rcc::PowerSource::SmpsDirect);
}

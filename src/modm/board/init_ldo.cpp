/*
 * Copyright (c) 2021, Christopher Durand
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
	// Configure internal voltage regulator
	Rcc::configurePowerSource(Rcc::PowerSource::Ldo);
}

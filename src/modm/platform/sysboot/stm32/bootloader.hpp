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

#pragma once
#include <stdint.h>

namespace modm::platform
{

/// @ingroup modm_platform_sysboot
struct SystemBootloader
{
	/// Resets the device and starts the system bootloader before any
	/// peripheral is initialized.
	[[noreturn]] static void
	reboot();

	/// Bootloader ID from the system memory (AN2606 §4.2): the upper digits
	/// encode the supported interfaces, the lowest digit the version, for
	/// example 0x92 or 0x121. Returns 0 if the device has no bootloader ID.
	static uint16_t
	id();
};

}	// namespace modm::platform

/*
 * Copyright (c) 2019, Ethan Slattery
 * Copyright (c) 2020, Erik Henriksson
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
#include "../device.hpp"

namespace modm::platform
{

enum class
ClockSource : uint32_t
{
	OSC8M = GCLK_GENCTRL_SRC_OSC8M_Val,
	DFLL48M = GCLK_GENCTRL_SRC_DFLL48M_Val
};

/**
 * Clock management
 *
 * \ingroup modm_platform_gclk
 */
class GenericClockController
{
public:
	template< uint32_t Core_Hz>
	static uint32_t
	setFlashLatency();

  template< uint32_t Core_Hz >
	static void
	updateCoreFrequency();

	static bool
	initOsc8MHz(uint32_t waitCycles = 2048);

	static bool
	initExternalCrystal(uint32_t waitCycles = 2048);

	static bool
	initDFLL48MHz(uint32_t waitCycles = 2048);

	static bool
	setSystemClock(
		ClockSource source = ClockSource::OSC8M,
		uint32_t waitCycles = 2048);

private:
	enum class
	ClockGenerator : uint32_t
	{
		System = 0,
		ExternalCrystal32K = 1,
		ULP32K = 2,
		Internal8M = 3,
	};

	enum class
	ClockMux : uint32_t
	{
		DFLL48M = 0,
	};
};

}

#include "gclk_impl.hpp"

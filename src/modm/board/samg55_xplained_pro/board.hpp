/*
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <modm/platform.hpp>

/// @ingroup modm_board_samg55_xplained_pro
namespace Board
{
using namespace modm::literals;
using namespace modm::platform;

struct SystemClock
{
	static constexpr uint32_t PllAMult = 3662;
	static constexpr uint32_t Frequency = PllAMult * SlowClkFreqHz;
	static bool inline
	enable()
	{
		ClockGen::setFlashLatency<Frequency>();
		ClockGen::updateCoreFrequency<Frequency>();
		ClockGen::enableExternal32Khz(false);
		ClockGen::enablePllA<PllAMult>();
		ClockGen::selectMasterClk<MasterClkSource::PLLA_CLK, MasterClkPrescaler::CLK_1>();
		return true;
	}
};

using Led = GpioA6;
using Button = GpioA2;

inline void
initialize()
{
	// Turn off the watchdog
	WDT->WDT_MR = (WDT_MR_WDDIS_Msk);

	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Led::setOutput(modm::Gpio::Low);

	Button::setInput();
}

} // namespace Board


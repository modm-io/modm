/*
 * Copyright (c) 2021, Tomasz Wasilczyk
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/architecture/interface/clock.hpp>
#include <modm/platform.hpp>

using namespace std::chrono_literals;
using namespace modm::platform;

struct SystemClock
{
	// Chosen to achieve 300MHz system clock
	static constexpr uint32_t PllAMult = 25;
	static constexpr uint32_t Frequency = 300'000'000;  // PllAMult * Osc 12MHz

	static bool inline
	enable()
	{
		ClockGen::setFlashLatency<Frequency / 2>();  // Flash runs off MCK

		ClockGen::enableMainInternal(MainInternalFreq::Rc12Mhz);
		ClockGen::selectMainClockSource(MainClockSource::Internal);
		ClockGen::enablePllA<PllAMult>();
		ClockGen::selectMasterClk<MasterClkSource::PLLA_CLK, MasterClkPrescaler::CLK_1, MasterClkDivider::Div2>();
		ClockGen::updateCoreFrequency<Frequency>();

		return true;
	}
};

using Led0 = GpioD17;
using Led1 = GpioD16;
using Led2 = GpioD15;

int
main()
{
	WDT->WDT_MR = (WDT_MR_WDDIS_Msk);  // turn off Watchdog

	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Led0::setOutput(false);
	Led1::setOutput(true);
	Led2::setOutput(false);

	while (1)
	{
		Led0::toggle();
		modm::delay(500ms);
	}

	return 0;
}

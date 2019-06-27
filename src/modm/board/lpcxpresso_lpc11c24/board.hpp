// coding: utf-8
/*
 * Copyright (c) 2019, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LPCXPRESSO_LPC11C24_HPP
#define MODM_LPCXPRESSO_LPC11C24_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

/// @ingroup modm_board_lpcxpresso_lpc11c24
namespace Board
{
using namespace modm::literals;

// Dummy clock for devices
struct SystemClock {
	static constexpr uint32_t Frequency = 48_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Ahb;
	static constexpr uint32_t Can = Apb;

	static bool inline
	enable()
	{
		// Set flash latency before switching to PLL
		// assuming that running on PLL is faster than before
		ClockControl::setFlashLatency<Frequency>();

		// enable external crystal
		ClockControl::enableExternalCrystal<12_MHz>();

		// Enable PLL
		// f_cco = 48 MHz * (M=4) = 192 MHz
		// f_cpu = f_cco   / (2 * P)
		//       = 192 MHz / (2 * 2) = 48 MHz
		// 156 MHz <= f_cco <= 320 MHz
		ClockControl::enablePll(ClockControl::PllSource::ExternalCrystal, /* pllM */ 4, /* pllP */1);

		// Switch system clock to external crystal
		// ClockControl::enableSystemClock(ClockControl::SystemClockSource::ExternalCrystal);

		// Switch system clock to PLL
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);

		// Set AHB to system clock, 48 MHz
		ClockControl::setAhbPrescaler(1);

		// Update frequencies for busy-wait delay functions
		ClockControl::updateCoreFrequency<Frequency>();

		return true;
	}
};

// using Button = GpioInputA0;

using LedRed = GpioOutput0_7;

using Leds = SoftwareGpioPort< LedRed >;


inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedRed::setOutput(modm::Gpio::Low);

	// Button::setInput();
	// Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	// Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

}

#endif	// MODM_LPCXPRESSO_LPC11C24_HPP

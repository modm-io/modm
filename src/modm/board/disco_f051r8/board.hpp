// coding: utf-8
/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F0_DISCOVERY_HPP
#define MODM_STM32_F0_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

/// @ingroup modm_board_disco_f051r8
namespace Board
{
	using namespace modm::literals;

/// STM32F0 running at 48MHz generated from the internal 8MHz with PLL.
struct SystemClock
{
	static constexpr int Frequency = 48_MHz;
	static constexpr int Usart1 = Frequency;
	static constexpr int Usart2 = Frequency;
	static constexpr int Spi2 = Frequency;

	static bool inline
	enable()
	{
		// enable internal 8 MHz HSI RC clock
		Rcc::enableInternalClock();
		// (internal clock / 2) * 12 = 48MHz
		const Rcc::PllFactors pllFactors{
			.pllMul = 12,
			.pllPrediv = 1  // only used with Hse
		};
		Rcc::enablePll(Rcc::PllSource::HsiDiv2, pllFactors);
		// set flash latency for 48MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};


using Button   = GpioInputA0;

using LedGreen = GpioOutputC9;
using LedBlue  = GpioOutputC8;

using Leds = SoftwareGpioPort< LedGreen, LedBlue >;


inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedBlue::setOutput(modm::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

} // namespace Board

#endif	// MODM_STM32_F0_DISCOVERY_HPP

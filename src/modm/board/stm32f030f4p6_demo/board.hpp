// coding: utf-8
/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F030F4P6_DEMO_BOARD_HPP
#define MODM_STM32_F030F4P6_DEMO_BOARD_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;


/// @ingroup modm_board_stm32f030_demo
namespace Board
{

/// STM32F030 running at 48MHz generated from the external 8MHz crystal
// Dummy clock for devices
struct systemClock {
	static constexpr uint32_t Frequency = MHz48;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Hsi = MHz8;
	static constexpr uint32_t Hsi14 = 14 * MHz1;

	static constexpr uint32_t Adc1 = Hsi14;

	static constexpr uint32_t Spi1 = Apb;

	static constexpr uint32_t Usart1 = Apb;

	static constexpr uint32_t I2c1   = Hsi;

	static constexpr uint32_t Timer1 = Apb;
	static constexpr uint32_t Timer3 = Apb;
	static constexpr uint32_t Timer14 = Apb;
	static constexpr uint32_t Timer16 = Apb;
	static constexpr uint32_t Timer17 = Apb;

	static bool inline
	enable()
	{
		Rcc::enableExternalCrystal();

		// external clock / 1 * 6 = 48MHz
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, 6, 1);

		// set flash latency for 48MHz
		Rcc::setFlashLatency<Frequency>();

		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);

		// AHB has max 48MHz
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);

		// APB1 has max. 48MHz
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);

		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

// User LED
using LedOrange = GpioOutputA4;
using Leds = SoftwareGpioPort< LedOrange >;

using Button = GpioUnused;

inline void
initialize()
{
	systemClock::enable();
	SysTickTimer::initialize<systemClock>();

	LedOrange::setOutput(modm::Gpio::Low);
}

} // Board namespace

#endif	// MODM_STM32_F030F4P6_DEMO_BOARD_HPP

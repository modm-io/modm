/*
 * Copyright (c) 2016-2018, 2024, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <modm/architecture.hpp>
#include <modm/platform.hpp>

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_c011f6
/// @{
using namespace modm::literals;

/// STM32C011F6 running at 48MHz generated from the internal clock
struct SystemClock
{
	static constexpr uint32_t Frequency = Rcc::HsiFrequency;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Adc1    = Apb;

	static constexpr uint32_t Spi1    = Apb;

	static constexpr uint32_t Usart1  = Apb;
	static constexpr uint32_t Usart2  = Apb;

	static constexpr uint32_t I2c1    = Apb;

	static constexpr uint32_t Timer1  = Apb;
	static constexpr uint32_t Timer2  = Apb;
	static constexpr uint32_t Timer3  = Apb;
	static constexpr uint32_t Timer14 = Apb;
	static constexpr uint32_t Timer16 = Apb;
	static constexpr uint32_t Timer17 = Apb;
	static constexpr uint32_t Iwdg    = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = 32.768_kHz;

	static bool inline
	enable()
	{
		Rcc::enableLowSpeedExternalCrystal();
		Rcc::enableRealTimeClock(Rcc::RealTimeClockSource::LowSpeedExternalCrystal);

		// 48MHz generated from internal RC
		Rcc::enableInternalClock();
		Rcc::setHsiSysDivider(Rcc::HsiSysDivider::Div1);
		// set flash latency for 48MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

using Button = GpioInputA14; // SWDCLK!
using LedA4 = GpioInverted<GpioOutputA4>;

using Leds = SoftwareGpioPort< LedA4 >;
/// @}

/// @ingroup modm_board_nucleo_c011f6
/// @{

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();
}
/// @}

}

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
#include <modm/debug.hpp>

using namespace modm::platform;

/// @ingroup modm_board_nucleo_c011f6
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_c011f6
/// @{
using namespace modm::literals;

/// STM32C011F6 running at 48MHz generated from the internal clock
struct SystemClock
{
	static constexpr uint32_t Lse = 32.768_kHz;
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
	static constexpr uint32_t Rtc     = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHsiClock(Rcc::HsiSysPrescaler::Div1);

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enableSystemClock(Rcc::SystemClockSource::HsiSys);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

using Button = GpioInputA14; // SWDCLK!
using LedA4 = GpioInverted<GpioOutputA4>;

using Leds = SoftwareGpioPort< LedA4 >;

using LoggerDevice = modm::IODeviceWrapper< Rtt<0>, modm::IOBuffer::DiscardIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();
}
/// @}

}

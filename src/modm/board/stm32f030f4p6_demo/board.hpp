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

#include <modm/architecture.hpp>
#include <modm/platform.hpp>
#include <modm/debug.hpp>

using namespace modm::platform;

/// @ingroup modm_board_stm32f030_demo
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_stm32f030_demo
/// @{
using namespace modm::literals;

/// STM32F030 running at 48MHz generated from the external 8MHz crystal
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr Rcc::PllConfig pll{.Mul = 6, .Prediv = 1};
	static constexpr uint32_t Pll = Hse * pll.Mul / pll.Prediv;
	static constexpr uint32_t Frequency = Pll;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Hsi = Rcc::HsiFrequency;
	static constexpr uint32_t Hsi14 = 14_MHz;

	static constexpr uint32_t Adc1 = Hsi14;

	static constexpr uint32_t Spi1 = Apb;

	static constexpr uint32_t Usart1 = Apb;

	static constexpr uint32_t I2c1 = Hsi;

	static constexpr uint32_t Timer1 = Apb;
	static constexpr uint32_t Timer3 = Apb;
	static constexpr uint32_t Timer14 = Apb;
	static constexpr uint32_t Timer16 = Apb;
	static constexpr uint32_t Timer17 = Apb;

	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Hse / 32;

	static bool inline
	enable()
	{
		Rcc::enableHseCrystal();

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enablePll(Rcc::PllSource::Hse, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Hse);
		return true;
	}
};

// User LED
using LedOrange = GpioInverted< GpioOutputA4 >;
using Leds = SoftwareGpioPort< LedOrange >;

using Button = GpioUnused;

using LoggerDevice = modm::IODeviceWrapper< Rtt<0>, modm::IOBuffer::DiscardIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedOrange::setOutput(modm::Gpio::Low);
}
/// @}

} // Board namespace

#endif	// MODM_STM32_F030F4P6_DEMO_BOARD_HPP

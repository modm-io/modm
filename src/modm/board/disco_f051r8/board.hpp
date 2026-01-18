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
#include <modm/debug/logger.hpp>

using namespace modm::platform;

/// @ingroup modm_board_disco_f051r8
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_disco_f051r8
/// @{
using namespace modm::literals;

/// STM32F0 running at 48MHz generated from the internal 8MHz with PLL.
struct SystemClock
{
	static constexpr Rcc::PllConfig pll{.Mul = 12, .Prediv = 1};
	static constexpr uint32_t Pll = Rcc::HsiFrequency / 2 * pll.Mul / pll.Prediv;
	static constexpr uint32_t Frequency = Pll;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Adc1 = Apb;

	static constexpr uint32_t Spi1 = Apb;
	static constexpr uint32_t Spi2 = Apb;

	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Usart2 = Apb;

	static constexpr uint32_t I2c1 = Apb;
	static constexpr uint32_t I2c2 = Apb;

	static constexpr uint32_t Timer1 = Apb;
	static constexpr uint32_t Timer2 = Apb;
	static constexpr uint32_t Timer3 = Apb;
	static constexpr uint32_t Timer6 = Apb;
	static constexpr uint32_t Timer14 = Apb;
	static constexpr uint32_t Timer15 = Apb;
	static constexpr uint32_t Timer16 = Apb;
	static constexpr uint32_t Timer17 = Apb;

	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableLsiClock();
		Rcc::enableHsiClock();

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enablePll(Rcc::PllSource::HsiDiv2, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lsi);
		return true;
	}
};

using Button   = GpioInputA0;

using LedGreen = GpioOutputC9;
using LedBlue  = GpioOutputC8;

using Leds = SoftwareGpioPort< LedGreen, LedBlue >;

using LoggerDevice = modm::IODeviceWrapper< Rtt<0>, modm::IOBuffer::DiscardIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedBlue::setOutput(modm::Gpio::Low);

	Button::setInput();
}
/// @}

} // namespace Board

#endif	// MODM_STM32_F0_DISCOVERY_HPP

/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture.hpp>
#include <modm/platform.hpp>
#include <modm/debug.hpp>

using namespace modm::platform;

/// @ingroup modm_board_weact_g030f6
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_weact_g030f6
/// @{
using namespace modm::literals;

/// STM32G030F6 running at 64MHz generated from the internal HSI16 oscillator
struct SystemClock
{
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr Rcc::PllConfig pll{
		.M = 1,  //  16 MHz /  1 =  16 MHz
		.N = 8,  //  16 MHz *  8 = 128 MHz
		.R = 2,  // 128 MHz /  2 =  64 MHz = F_cpu
	};
	static constexpr uint32_t PllR = Rcc::HsiFrequency / pll.M * pll.N / pll.R;
	static_assert(PllR == Rcc::MaxFrequency);

	static constexpr uint32_t SysClk = PllR;
	static constexpr uint32_t Frequency = SysClk;
	static constexpr uint32_t Ahb = SysClk;
	static constexpr uint32_t Apb = SysClk;
	static constexpr uint32_t ApbTimer = Apb;

	static constexpr uint32_t Crc = Ahb;
	static constexpr uint32_t Dma1 = Ahb;
	static constexpr uint32_t DmaMux = Ahb;
	static constexpr uint32_t Exti = Ahb;
	static constexpr uint32_t Flash = Ahb;

	static constexpr uint32_t Adc = Apb;
	static constexpr uint32_t Dbg = Apb;
	static constexpr uint32_t I2c1 = Apb;
	static constexpr uint32_t I2c2 = Apb;
	static constexpr uint32_t Pwr = Apb;
	static constexpr uint32_t Spi1 = Apb;
	static constexpr uint32_t Spi2 = Apb;
	static constexpr uint32_t Syscfg = Apb;
	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Usart2 = Apb;
	static constexpr uint32_t Wwdg = Apb;

	static constexpr uint32_t Timer1 = ApbTimer;
	static constexpr uint32_t Timer3 = ApbTimer;
	static constexpr uint32_t Timer14 = ApbTimer;
	static constexpr uint32_t Timer16 = ApbTimer;
	static constexpr uint32_t Timer17 = ApbTimer;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::setFlashLatency<Frequency>();

		Rcc::enablePll(Rcc::PllSource::Hsi16, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::PllR);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);

		Rcc::updateCoreFrequency<Frequency>();

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

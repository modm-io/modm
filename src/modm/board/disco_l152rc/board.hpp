/*
 * Copyright (c) 2019, Niklas Hauser
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
#include <modm/architecture.hpp>
#include <modm/debug.hpp>

using namespace modm::platform;

/// @ingroup modm_board_disco_l152rc
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_disco_l152rc
/// @{
using namespace modm::literals;

/// STM32L152RC running at 32MHz generated from the internal 16MHz clock
struct SystemClock
{
	static constexpr uint32_t Lse = 32.768_kHz;
	// VCO is 64 MHz, USB requires 96 MHz
	static constexpr Rcc::PllConfig pll{.Mul = Rcc::PllMultiplier::Mul4, .Div = 2};
	static constexpr uint32_t Pll = Rcc::HsiFrequency * Rcc::value(pll.Mul) / pll.Div;
	static_assert(Pll == Rcc::MaxFrequency);

	static constexpr uint32_t Frequency = Pll;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Crc = Ahb;
	static constexpr uint32_t Dma1 = Ahb;
	static constexpr uint32_t Dma2 = Ahb;
	static constexpr uint32_t Flash = Ahb;

	static constexpr uint32_t Comp = Apb1;
	static constexpr uint32_t Dac = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Lcd = Apb1;
	static constexpr uint32_t Opamp = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t Ri = Apb1;
	static constexpr uint32_t RtcBus = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Wwdg = Apb1;

	static constexpr uint32_t Adc = Apb2;
	static constexpr uint32_t Adc1 = Apb2;
	static constexpr uint32_t Exti = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Syscfg = Apb2;
	static constexpr uint32_t Usart1 = Apb2;

	static constexpr uint32_t Apb1Timer = Apb1;
	static constexpr uint32_t Apb2Timer = Apb2;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer9  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;

	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHsiClock();

		Rcc::setVoltageScaling(Rcc::VoltageScaling::Range1);
		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);

		// The SYSCLK may increase at most 4x per step with 5us in between (RM0038 6.2.7)
		Rcc::enableMsiClock(Rcc::MsiRange::MHz4);
		modm::delay_us(5);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Hsi);
		modm::delay_us(5);
		Rcc::enablePll(Rcc::PllSource::Hsi, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

using Button = GpioInputA0;
using LedBlue = GpioOutputB6;
using LedGreen = GpioOutputB7;

using Leds = SoftwareGpioPort< LedBlue, LedGreen >;

using LoggerDevice = modm::IODeviceWrapper< Rtt<0>, modm::IOBuffer::DiscardIfFull >;
/// @}

namespace lcd
{
/// @ingroup modm_board_disco_l152rc
/// @{
using Com0 = GpioOutputA8;
using Com1 = GpioOutputA9;
using Com2 = GpioOutputA10;
using Com3 = GpioOutputB9;

using Seg0 = GpioOutputA1;
using Seg1 = GpioOutputA2;
using Seg2 = GpioOutputA3;
using Seg7 = GpioOutputB3;
using Seg8 = GpioOutputB4;
using Seg9 = GpioOutputB5;
using Seg10 = GpioOutputB10;
using Seg11 = GpioOutputB11;
using Seg12 = GpioOutputB12;
using Seg13 = GpioOutputB13;
using Seg14 = GpioOutputB14;
using Seg15 = GpioOutputB15;
using Seg16 = GpioOutputB8;
using Seg17 = GpioOutputA15;
using Seg18 = GpioOutputC0;
using Seg19 = GpioOutputC1;
using Seg20 = GpioOutputC2;
using Seg21 = GpioOutputC3;
using Seg24 = GpioOutputC6;
using Seg25 = GpioOutputC7;
using Seg26 = GpioOutputC8;
using Seg27 = GpioOutputC9;
using Seg28 = GpioOutputC10;
using Seg29 = GpioOutputC11;
/// @}
}

/// @ingroup modm_board_disco_l152rc
inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedBlue::setOutput(modm::Gpio::Low);
	LedGreen::setOutput(modm::Gpio::Low);

	Button::setInput();
}

}

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
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

/// @ingroup modm_board_disco_l152rc
namespace Board
{
using namespace modm::literals;

/// STM32L152RC running at 32MHz generated from the external 8MHz ST-Link clock
// Dummy clock for devices
struct SystemClock
{
	static constexpr uint32_t Frequency = 32_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc    = Apb2;

	static constexpr uint32_t Spi1   = Apb2;
	static constexpr uint32_t Spi2   = Apb1;
	static constexpr uint32_t Spi3   = Apb1;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Usart4 = Apb1;
	static constexpr uint32_t Usart5 = Apb1;

	static constexpr uint32_t I2c1   = Apb1;
	static constexpr uint32_t I2c2   = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 1;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer9  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;

	static bool inline
	enable()
	{
		// Enable power scaling for 1.8V
		PWR->CR = (PWR->CR & ~PWR_CR_VOS) | PWR_CR_VOS_0;
		while(PWR->CSR & PWR_CSR_VOSF) ;

		/* From RM0038: 6.2.7 System clock (SYSCLK) selection (page ~135)
		 *
		 * The SYSCLK frequency change has to follow the rule that the final
		 * frequency is less then 4 x initial frequency to limit the VCORE drop
		 * due to a current consumption peak when the frequency increases. It
		 * must also respect 5 μs delay between two changes.
		 *
		 * For example to switch from 2.1 MHz to 32 MHz, the user can switch
		 * from 2.1 MHz to 8 MHz, wait 5 μs, then switch from 8 MHz to 32 MHz.
		 */

		Rcc::setFlashLatency<4.1_MHz>();
		Rcc::updateCoreFrequency<4.1_MHz>();
		Rcc::enableMultiSpeedInternalClock(Rcc::MsiFrequency::MHz4);
		modm::delay_us(5);

		Rcc::enableInternalClock();	// 16MHz
		// set flash latency
		Rcc::setFlashLatency<16_MHz>();
		// switch system clock to HSE output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Hsi);
		Rcc::updateCoreFrequency<16_MHz>();
		modm::delay_us(5);

		// internal clock 16MHz * 4 / 2 = 32MHz => bad for USB
		const Rcc::PllFactors pllFactors{
			.pllMul = Rcc::PllMultiplier::Mul4,
			.pllDiv = 2,
		};
		Rcc::enablePll(Rcc::PllSource::InternalClock, pllFactors);
		// set flash latency
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		// All busses max. 32MHz
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);

		return true;
	}
};

using Button = GpioInputA0;
using LedBlue = GpioOutputB6;
using LedGreen = GpioOutputB7;

using Leds = SoftwareGpioPort< LedBlue, LedGreen >;

namespace lcd
{
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
}

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

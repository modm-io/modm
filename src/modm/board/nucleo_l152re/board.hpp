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
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_l152re
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_l152re
/// @{
using namespace modm::literals;

/// STM32L152RE running at 32MHz generated from the external 8MHz ST-Link clock
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

		// MCO from ST-Link
		Rcc::enableExternalClock();	// 8MHz
		// set flash latency
		Rcc::setFlashLatency<8_MHz>();
		// switch system clock to HSE output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Hse);
		Rcc::updateCoreFrequency<8_MHz>();

		// internal clock 8MHz * 8 / 2 = 32MHz => bad for USB
		const Rcc::PllFactors pllFactors{
			.pllMul = Rcc::PllMultiplier::Mul8,
			.pllDiv = 2,
		};
		Rcc::enablePll(Rcc::PllSource::ExternalClock, pllFactors);
		// set flash latency
		Rcc::setFlashLatency<Frequency>();
		modm::delay_us(5);
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

// Arduino Footprint
#include "nucleo64_arduino.hpp"

using Button = GpioInverted<GpioInputC13>;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_l152re
/// @{
using Rx = GpioInputA3;
using Tx = GpioOutputA2;
using Uart = Usart2;
/// @}
}

/// @ingroup modm_board_nucleo_l152re
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	Button::setInput();
}
/// @}

}

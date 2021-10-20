/*
 * Copyright (c) 2021, Niklas Hauser
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
#include <modm/debug.hpp>
/// @ingroup modm_board_nucleo_l053r8
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_nucleo_l053r8
namespace Board
{
using namespace modm::literals;

/// STM32L053R8 running at 32MHz generated from 16 MHz HSI16 clock
struct SystemClock
{
	static constexpr uint32_t Frequency = 32_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc     = Apb2;

	static constexpr uint32_t Dac     = Apb1;

	static constexpr uint32_t Comp1   = Apb2;
	static constexpr uint32_t Comp2   = Apb2;

	static constexpr uint32_t Spi1    = Apb2;
	static constexpr uint32_t Spi2    = Apb1;

	static constexpr uint32_t Usart1  = Apb2;
	static constexpr uint32_t Usart2  = Apb1;

	static constexpr uint32_t Lpuart1 = Apb1;

	static constexpr uint32_t I2c1    = Apb1;
	static constexpr uint32_t I2c2    = Apb1;

	static constexpr uint32_t Usb     = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 1;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer21 = Apb2Timer;
	static constexpr uint32_t Timer22 = Apb2Timer;

	static bool inline
	enable()
	{
		Rcc::enableInternalClock();	// 16MHz
		// (internal clock / 1) * 4 / 2 = 32MHz
		const Rcc::PllFactors pllFactors{
			.pllMul = Rcc::PllMultiplier::Mul4,
			.pllDiv = 2,
			.enableHsiPrediv4 = false
		};
		Rcc::enablePll(Rcc::PllSource::Hsi16, pllFactors);
		// set flash latency for 32MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

// Arduino Footprint
#include "nucleo64_arduino.hpp"

using Button = GpioInverted<GpioInputC13>;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;


namespace stlink
{
using Rx = GpioInputA3;
using Tx = GpioOutputA2;
using Uart = Usart2;
}

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

}

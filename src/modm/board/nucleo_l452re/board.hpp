// coding: utf-8
/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_NUCLEO_L452RE_HPP
#define MODM_STM32_NUCLEO_L452RE_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_l452re
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_nucleo_l452re
namespace Board
{
	using namespace modm::literals;

/// STM32L4 running at 80MHz generated from the
/// internal high speed oscillator

// Dummy clock for devices
struct SystemClock {
	static constexpr uint32_t Frequency = 80_MHz;
	static constexpr uint32_t Ahb  = Frequency;
	static constexpr uint32_t Ahb2 = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc1 = Ahb2;

	static constexpr uint32_t Can1 = Apb1;

	static constexpr uint32_t Comp1 = Apb2;
	static constexpr uint32_t Comp2 = Apb2;

	static constexpr uint32_t Dac1 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;

	static constexpr uint32_t Spi1 = Apb2;

	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;

	static constexpr uint32_t Timer1 = Apb2;

	static constexpr uint32_t Timer2 = Apb1;
	static constexpr uint32_t Timer6 = Apb1;

	static constexpr uint32_t Timer15 = Apb2;
	static constexpr uint32_t Timer16 = Apb2;

	static constexpr uint32_t Uart4 = Apb1;

	static constexpr uint32_t Usart1 = Apb2;

	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;

	static constexpr uint32_t Usb = Apb1;

	static bool inline
	enable()
	{
		Rcc::enableInternalClock();	// 16MHz
		Rcc::PllFactors pllFactors{
			.pllM = 1,	//  16MHz / M= 1 ->  16MHz
			.pllN = 10,	//  16MHz * N=10 -> 160MHz
			.pllR = 2,	// 160MHz / R= 2 ->  80MHz = F_cpu
		};
		Rcc::enablePll(Rcc::PllSource::InternalClock, pllFactors);
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		// APB1 has max. 80MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

// Arduino Footprint
#include "nucleo64_arduino.hpp"

// Button connects to GND
using Button = GpioInverted<GpioInputC13>;

// User LD2
using LedGreen = GpioOutputA5;
using Leds = SoftwareGpioPort< LedGreen >;

namespace stlink
{
using Tx = GpioOutputA2;
using Rx = GpioInputA3;
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

	LedGreen::setOutput(modm::Gpio::Low);

	Button::setInput();
}

} // Board namespace

#endif	// MODM_STM32_NUCLEO_L452RE_HPP

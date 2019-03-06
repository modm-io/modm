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

#ifndef MODM_STM32_NUCLEO_L476RG_HPP
#define MODM_STM32_NUCLEO_L476RG_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_l476rg
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_nucleo_l476rg
namespace Board
{
	using namespace modm::literals;

/// STM32L4 running at 48MHz generated from the
/// internal Multispeed oscillator

// Dummy clock for devices
struct SystemClock {
	static constexpr uint32_t Frequency = 48_MHz;
	static constexpr uint32_t Ahb  = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;

	static constexpr uint32_t Spi1 = Apb2;

	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;

	static constexpr uint32_t Can1 = Apb1;

	static constexpr uint32_t Usart1 = Apb2;

	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Usart4 = Apb1;
	static constexpr uint32_t Usart5 = Apb1;

	static bool inline
	enable()
	{
		// set flash latency first because system already runs from MSI
		Rcc::setFlashLatency<Frequency>();
		Rcc::enableMultiSpeedInternalClock(Rcc::MsiFrequency::MHz48);
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

#endif	// MODM_STM32_NUCLEO_L476RG_HPP

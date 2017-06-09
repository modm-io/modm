// coding: utf-8
/*
 * Copyright (c) 2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

//
// NUCLEO-L476RG
// Nucleo kit for STM32L476RG
// http://www.st.com/en/evaluation-tools/nucleo-l476rg.html
//

#ifndef MODM_STM32_NUCLEO_L476RG_HPP
#define MODM_STM32_NUCLEO_L476RG_HPP

#include <modm/architecture/platform.hpp>
#include <modm/debug/logger.hpp>
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{

/// STM32L4 running at 48MHz generated from the
/// internal Multispeed oscillator

// Dummy clock for devices
struct systemClock {
	static constexpr uint32_t Frequency = 48 * MHz1;
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
		ClockControl::setFlashLatency(Frequency);

		ClockControl::enableMultiSpeedInternalClock(ClockControl::MsiFrequency::MHz48);

		modm::clock::fcpu     = Frequency;
		modm::clock::fcpu_kHz = Frequency / 1000;
		modm::clock::fcpu_MHz = Frequency / 1000000;
		modm::clock::ns_per_loop = ::round(3000.f / (Frequency / 1000000));

		return true;
	}
};

// Arduino Footprint
#include "../nucleo64_arduino.hpp"

// Button connects to GND
using Button = modm::GpioInverted<GpioInputC13>;

// User LD2
using LedGreen = GpioOutputA5;
using Leds = modm::SoftwareGpioPort< LedGreen >;

namespace stlink
{
using Tx = GpioOutputA2;
using Rx = GpioInputA3;
using Uart = Usart2;
}

inline void
initialize()
{
	systemClock::enable();
	modm::cortex::SysTickTimer::initialize<systemClock>();

	stlink::Tx::connect(stlink::Uart::Tx);
	stlink::Rx::connect(stlink::Uart::Rx, Gpio::InputType::PullUp);
	stlink::Uart::initialize<systemClock, 115200>(12);

	LedGreen::setOutput(modm::Gpio::Low);

	Button::setInput();
}

} // Board namespace

#endif	// MODM_STM32_NUCLEO_L476RG_HPP

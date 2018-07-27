/*
 * Copyright (c) 2016-2018, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Carl Treudler
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

//
// NUCLEO-F042K6
// Nucleo kit for STM32F042K6
// http://www.st.com/en/evaluation-tools/nucleo-f042k6.html
//

#ifndef MODM_STM32_NUCLEO_F042K6_HPP
#define MODM_STM32_NUCLEO_F042K6_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;


namespace Board
{

/// STM32F042K6 running at 48MHz generated from the internal 8MHz crystal
// Dummy clock for devices
struct systemClock {
	static constexpr uint32_t Frequency = MHz48;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Adc1   = Apb;

	static constexpr uint32_t Spi1   = Apb;

	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Usart2 = Apb;

	static constexpr uint32_t I2c1   = Apb;

	static constexpr uint32_t Timer1  = Apb;
	static constexpr uint32_t Timer2  = Apb;
	static constexpr uint32_t Timer3  = Apb;
	static constexpr uint32_t Timer14 = Apb;
	static constexpr uint32_t Timer16 = Apb;
	static constexpr uint32_t Timer17 = Apb;

	static bool inline
	enable()
	{
		ClockControl::enableInternalClock();	// 8MHz
		// (internal clock / 2) * 12 = 48MHz
		ClockControl::enablePll(ClockControl::PllSource::InternalClock, ClockControl::UsbPrescaler::Div1, 12, 2, 1);
		// set flash latency for 48MHz
		ClockControl::setFlashLatency(Frequency);
		// switch system clock to PLL output
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		ClockControl::setApbPrescaler(ClockControl::ApbPrescaler::Div1);
		// update frequencies for busy-wait delay functions
		modm::clock::fcpu     = Frequency;
		modm::clock::fcpu_kHz = Frequency / 1000;
		modm::clock::fcpu_MHz = Frequency / 1000000;
		modm::clock::ns_per_loop = ::round(4000.f / (Frequency / 1000000));

		return true;
	}
};

// Arduino Nano Footprint
#include "nucleo32_arduino.hpp"

using Button = GpioUnused;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;


namespace stlink
{
	using Rx = GpioInputA15;
	using Tx = GpioOutputA2;

	using Uart = Usart2;
}

using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;


inline void
initialize()
{
	systemClock::enable();
	modm::cortex::SysTickTimer::initialize<systemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<systemClock, 115200>();
}

}

#endif	// MODM_STM32_NUCLEO_F042K6_HPP

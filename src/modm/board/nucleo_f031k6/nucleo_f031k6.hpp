/*
 * Copyright (c) 2016, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

//
// NUCLEO-F031K6
// Nucleo kit for STM32F031K6
// http://www.st.com/en/evaluation-tools/nucleo-f031k6.html
//

#ifndef MODM_STM32_NUCLEO_F031K6_HPP
#define MODM_STM32_NUCLEO_F031K6_HPP

#include <modm/architecture/platform.hpp>
#include <modm/debug/logger.hpp>
#define MODM_BOARD_HAS_LOGGER

using namespace modm::stm32;


namespace Board
{

/// STM32F031K6 running at 48MHz generated from the internal 8MHz crystal
// Dummy clock for devices
struct systemClock {
	static constexpr uint32_t Frequency = MHz48;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Adc1   = Apb;

	static constexpr uint32_t Spi1   = Apb;

	static constexpr uint32_t Usart1 = Apb;

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
		ClockControl::enablePll(ClockControl::PllSource::InternalClock, 12, 1);
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
using A0 = GpioA0;
using A1 = GpioA1;
using A2 = GpioA3;
using A3 = GpioA4;
using A4 = GpioA5;
using A5 = GpioA6;
using A6 = GpioA7;
using A7 = GpioA2;

using D0  = GpioA10;
using D1  = GpioA9;
using D2  = GpioA12;
using D3  = GpioB0;
using D4  = GpioB7;
using D5  = GpioB6;
using D6  = GpioB1;
using D7  = GpioF0;
using D8  = GpioF1;
using D9  = GpioA8;
using D10 = GpioA11;
using D11 = GpioB5;
using D12 = GpioB4;
using D13 = GpioB3;

using Button = modm::GpioUnused;
using LedD13 = D13;

using Leds = modm::SoftwareGpioPort< LedD13 >;


namespace stlink
{
using Rx = GpioInputA15;
using Tx = GpioOutputA2;
using Uart = Usart1;
}


inline void
initialize()
{
	systemClock::enable();
	modm::cortex::SysTickTimer::initialize<systemClock>();

	stlink::Tx::connect(stlink::Uart::Tx);
	stlink::Rx::connect(stlink::Uart::Rx, Gpio::InputType::PullUp);
	stlink::Uart::initialize<systemClock, 115200>(12);
}

}

#endif	// MODM_STM32_NUCLEO_F031K6_HPP

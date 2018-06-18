// coding: utf-8
/* Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

//
// NUCLEO-L432KC
// Nucleo kit for STM32L432KC
// http://www.st.com/en/evaluation-tools/nucleo-l432kc.html
//

#ifndef MODM_STM32_NUCLEO_L432KC_HPP
#define MODM_STM32_NUCLEO_L432KC_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{

/// STM32L4 running at 80MHz generated with the PLL from 4MHz MSI clock

struct systemClock {
	static constexpr uint32_t Frequency = 80 * MHz1;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Usart1 = Apb2;

	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Usart4 = Apb1;
	static constexpr uint32_t Usart5 = Apb1;

	static bool inline
	enable()
	{
		ClockControl::enablePll(
			ClockControl::PllSource::MultiSpeedInternalClock,
			1,	// 4MHz / M=1 -> 4MHz
			40,	// 4MHz * N=40 -> 160MHz <= 344MHz = PLL VCO output max, >= 64 MHz = PLL VCO out min
			2,	// 160MHz / P=2 -> 80MHz = F_cpu
			2	// 80MHz / Q=2 -> 40MHz = F_usb
		);
		ClockControl::setFlashLatency(Frequency);

		// switch system clock to PLL output
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		// APB1 has max. 80MHz
		ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div1);
		ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		modm::clock::fcpu     = Frequency;
		modm::clock::fcpu_kHz = Frequency / 1000;
		modm::clock::fcpu_MHz = Frequency / 1000000;
		modm::clock::ns_per_loop = ::round(3000.f / (Frequency / 1000000));

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
// using D7  = GpioF0; // NC
// using D8  = GpioF1; // NC
using D9  = GpioA8;
using D10 = GpioA11;
using D11 = GpioB5;
using D12 = GpioB4;
using D13 = GpioB3;


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

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>(Gpio::InputType::PullUp);
	stlink::Uart::initialize<systemClock, 115200>();
}

}

#endif	// MODM_STM32_NUCLEO_L432KC_HPP

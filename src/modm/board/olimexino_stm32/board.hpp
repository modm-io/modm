/*
 * Copyright (c) 2017, Carl Treudler
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

//
// Olimexino STM32 (modified Maple board with STM32F103RBT6)
// https://www.olimex.com/Products/Duino/STM32/OLIMEXINO-STM32/open-source-hardware
//

#ifndef MODM_STM32_OLIMEXINO_STM32_HPP
#define MODM_STM32_OLIMEXINO_STM32_HPP

#include <modm/platform/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{

/// STM32F103RB running at 64MHz generated from the internal 8MHz crystal
// Dummy clock for devices
struct systemClock {
	static constexpr uint32_t Frequency = MHz64;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 2;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc1   = Apb2;
	static constexpr uint32_t Adc2   = Apb2;
	static constexpr uint32_t Adc3   = Apb2;

	static constexpr uint32_t Spi1   = Apb2;
	static constexpr uint32_t Spi2   = Apb1;
	static constexpr uint32_t Spi3   = Apb1;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;

	static constexpr uint32_t Can1   = Apb1;

	static constexpr uint32_t I2c1   = Apb1;
	static constexpr uint32_t I2c2   = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;

	static bool inline
	enable()
	{
		ClockControl::enableInternalClock();	// 8MHz
		// internal clock / 2 * 16 = 64MHz, => 64/1.5 = 42.6 => bad for USB
		ClockControl::enablePll(ClockControl::PllSource::InternalClock, ClockControl::UsbPrescaler::Div1_5, 16);
		// set flash latency for 64MHz
		ClockControl::setFlashLatency(Frequency);
		// switch system clock to PLL output
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		// APB1 has max. 36MHz
		ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div2);
		ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		modm::clock::fcpu     = Frequency;
		modm::clock::fcpu_kHz = Frequency / 1000;
		modm::clock::fcpu_MHz = Frequency / 1000000;
		modm::clock::ns_per_loop = ::round(3000.f / (Frequency / 1000000));

		return true;
	}
};

// Arduino Footprint
using A0 = GpioC0;
using A1 = GpioC1;
using A2 = GpioC2;
using A3 = GpioC3;

using A4 = GpioC4;
using A5 = GpioC5;

using D0  = GpioA3;
using D1  = GpioA2;
using D2  = GpioA0;
using D3  = GpioA1;
using D4  = GpioB5;
using D5  = GpioB6;
using D6  = GpioA8;
using D7  = GpioA9;
using D8  = GpioA10;
using D9  = GpioB7;
using D10 = GpioA4;
using D11 = GpioA7;
using D12 = GpioA6;
using D13 = GpioA5;
using D14 = GpioB8;

using CANCTRL = GpioC13;
using CANTX  =  GpioB9;
using CANRX  =  GpioB8;

using LedD13 = D13;
using LedD3 = D3;

using Leds = SoftwareGpioPort< LedD13, LedD3 >;

namespace uext
{
using Rx = GpioInputA10;
using Tx = GpioOutputA9;
using Uart = Usart1;
}

namespace Sdcard {
	using nCS  = GpioD2;
	using sck = GpioB13;
	using miso = GpioB14;
	using mosi = GpioB15;
	using SpiMaster = SpiMaster2;
}

using LoggerDevice = modm::IODeviceWrapper< uext::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	systemClock::enable();
	modm::cortex::SysTickTimer::initialize<systemClock>();

	uext::Uart::connect<uext::Tx::Tx, uext::Rx::Rx>();
	uext::Uart::initialize<systemClock, modm::Uart::Baudrate::B115200>(12);

}

} // Board namespace

#endif	// MODM_STM32_OLIMEXINO_STM32_HPP

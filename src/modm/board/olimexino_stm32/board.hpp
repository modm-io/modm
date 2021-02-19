/*
 * Copyright (c) 2017, Carl Treudler
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_STM32_OLIMEXINO_STM32_HPP
#define MODM_STM32_OLIMEXINO_STM32_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_olimexino_stm32
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_olimexino_stm32
namespace Board
{
	using namespace modm::literals;

/// STM32F103RB running at 64MHz generated from the internal 8MHz crystal
// Dummy clock for devices
struct SystemClock {
	static constexpr uint32_t Frequency = 64_MHz;
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
		Rcc::enableInternalClock();	// 8MHz
		// internal clock / 2 * 16 = 64MHz, => 64/1.5 = 42.6 => bad for USB
		const Rcc::PllFactors pllFactors{
			.pllMul = 16,
		};
		Rcc::enablePll(Rcc::PllSource::InternalClock, pllFactors);
		// set flash latency for 64MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		// APB1 has max. 36MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

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
using Button = GpioUnused;

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
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	uext::Uart::connect<uext::Tx::Tx, uext::Rx::Rx>();
	uext::Uart::initialize<SystemClock, 115200_Bd>();

}

} // Board namespace

#endif	// MODM_STM32_OLIMEXINO_STM32_HPP

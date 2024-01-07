/*
 * Copyright (c) 2016-2018, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_STM32_NUCLEO_L031K6_HPP
#define MODM_STM32_NUCLEO_L031K6_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_l031k6
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_l031k6
/// @{
using namespace modm::literals;

/// STM32L031K6 running at 32MHz generated from 16 MHz HSI16 clock
struct SystemClock
{
	static constexpr uint32_t Frequency = 32_MHz;
	static constexpr uint32_t Ahb  = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Apb1Timer = Apb1;
	static constexpr uint32_t Apb2Timer = Apb2;

	static constexpr uint32_t Adc = Apb2;

	static constexpr uint32_t Comp1 = Apb2;
	static constexpr uint32_t Comp2 = Apb2;

	static constexpr uint32_t I2c1 = Apb1;

	static constexpr uint32_t Spi1 = Apb2;

	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer21 = Apb2Timer;
	static constexpr uint32_t Timer22 = Apb2Timer;

	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

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

// Arduino Nano Footprint
#include "nucleo32_arduino.hpp"

using Button = GpioUnused;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_l031k6
/// @{
using Rx = GpioInputA15;
using Tx = GpioOutputA2;
using Uart = Usart2;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_l031k6
/// @{
using Sda = GpioB7;
using Scl = GpioB6;
using Controller = I2cMaster1;
/// @}
}

namespace spi
{
/// @ingroup modm_board_nucleo_l031k6
/// @{
using Cs = GpioA11;
using Sck = GpioB3;
using Sdi = GpioB4;
using Sdo = GpioB5;
using Controller = SpiMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_l031k6
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();
}

inline void
initializeI2c()
{
	i2c::Controller::connect<i2c::Sda::Sda, i2c::Scl::Scl>();
	i2c::Controller::initialize<SystemClock, 400_kHz>();
}

inline void
initializeSpi()
{
	spi::Controller::connect<spi::Sck::Sck, spi::Sdo::Mosi, spi::Sdi::Miso>();
	spi::Controller::initialize<SystemClock, 4_MHz>();
}
/// @}

}

#endif	// MODM_STM32_NUCLEO_L031K6_HPP

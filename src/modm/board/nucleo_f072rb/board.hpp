/*
 * Copyright (c) 2016-2018, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_STM32_NUCLEO_F072RB_HPP
#define MODM_STM32_NUCLEO_F072RB_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_f072rb
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_f072rb
/// @{
using namespace modm::literals;

/// STM32f072rb running at 48MHz generated from the internal 8MHz crystal
struct SystemClock
{
	static constexpr int Frequency = 48_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Adc   = Apb;
	static constexpr uint32_t Can   = Apb;

	static constexpr uint32_t Spi1   = Apb;
	static constexpr uint32_t Spi2   = Apb;

	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Usart2 = Apb;
	static constexpr uint32_t Usart3 = Apb;
	static constexpr uint32_t Usart4 = Apb;

	static constexpr uint32_t I2c1   = Apb;
	static constexpr uint32_t I2c2   = Apb;

	static constexpr uint32_t Timer1  = Apb;
	static constexpr uint32_t Timer2  = Apb;
	static constexpr uint32_t Timer3  = Apb;
	static constexpr uint32_t Timer6  = Apb;
	static constexpr uint32_t Timer7  = Apb;
	static constexpr uint32_t Timer14 = Apb;
	static constexpr uint32_t Timer15 = Apb;
	static constexpr uint32_t Timer16 = Apb;
	static constexpr uint32_t Timer17 = Apb;

	static constexpr uint32_t Usb = 48_MHz;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		// Enable the internal 48MHz clock
		Rcc::enableInternalClockMHz48();
		// set flash latency for 48MHz
		Rcc::setFlashLatency<Frequency>();
		// Switch to the 48MHz clock
		Rcc::enableSystemClock(Rcc::SystemClockSource::InternalClockMHz48);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

// Arduino Uno Footprint
#include "nucleo64_arduino.hpp"

using Button = GpioInverted<GpioInputC13>;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_f072rb
/// @{
using Rx = GpioInputA3;
using Tx = GpioOutputA2;
using Uart = Usart2;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_f072rb
/// @{
using Sda = D14;
using Scl = D15;
using Controller = I2cMaster1;
/// @}
}

namespace spi
{
/// @ingroup modm_board_nucleo_f072rb
/// @{
using Cs = D10;
using Sck = D13;
using Sdi = D12;
using Sdo = D11;
using Controller = SpiMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_f072rb
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
	spi::Controller::initialize<SystemClock, 3_MHz>();
}
/// @}

}

#endif	// MODM_STM32_NUCLEO_F072RB_HPP

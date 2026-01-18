/*
 * Copyright (c) 2016-2018, 2024, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_STM32_NUCLEO_C031C6_HPP
#define MODM_STM32_NUCLEO_C031C6_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_c031c6
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_c031c6
/// @{
using namespace modm::literals;

/// STM32C031C6 running at 48MHz generated from the external HSE crystal
struct SystemClock
{
	static constexpr uint32_t Hse = 48_MHz;
	static constexpr uint32_t Frequency = Hse;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Adc1    = Apb;

	static constexpr uint32_t Spi1    = Apb;

	static constexpr uint32_t Usart1  = Apb;
	static constexpr uint32_t Usart2  = Apb;

	static constexpr uint32_t I2c1    = Apb;

	static constexpr uint32_t Timer1  = Apb;
	static constexpr uint32_t Timer2  = Apb;
	static constexpr uint32_t Timer3  = Apb;
	static constexpr uint32_t Timer14 = Apb;
	static constexpr uint32_t Timer16 = Apb;
	static constexpr uint32_t Timer17 = Apb;
	static constexpr uint32_t Iwdg    = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc     = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enableSystemClock(Rcc::SystemClockSource::Hse);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

// Arduino Footprint
#include "nucleo64_arduino.hpp"

using Button = GpioInverted<GpioInputC13>;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_c031c6
/// @{
using Rx = GpioInputA3;
using Tx = GpioOutputA2;
using Uart = BufferedUart<UsartHal2, UartTxBuffer<64>>;
/// @}
}

/// @ingroup modm_board_nucleo_c031c6
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	Button::setInput(Gpio::InputType::PullDown);
}
/// @}

}

#endif	// MODM_STM32_NUCLEO_C031C6_HPP

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

#ifndef MODM_STM32_NUCLEO_L432KC_HPP
#define MODM_STM32_NUCLEO_L432KC_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_l432kc
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_l432kc
/// @{
using namespace modm::literals;

/// STM32L4 running at 80MHz generated with the PLL from 4MHz MSI clock
struct SystemClock
{
	static constexpr uint32_t Frequency = 80_MHz;
	static constexpr uint32_t Ahb  = Frequency;
	static constexpr uint32_t Ahb2 = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc1 = Ahb2;

	static constexpr uint32_t Can1 = Apb1;

	static constexpr uint32_t Comp1 = Apb2;
	static constexpr uint32_t Comp2 = Apb2;

	static constexpr uint32_t Dac1 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;

	static constexpr uint32_t Spi1 = Apb2;

	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;

	static constexpr uint32_t Timer1 = Apb2;

	static constexpr uint32_t Timer2 = Apb1;
	static constexpr uint32_t Timer6 = Apb1;

	static constexpr uint32_t Timer15 = Apb2;
	static constexpr uint32_t Timer16 = Apb2;

	static constexpr uint32_t Uart4 = Apb1;

	static constexpr uint32_t Usart1 = Apb2;

	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb2;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Usb = Apb1;

	static bool inline
	enable()
	{
		const Rcc::PllFactors pllFactors{
			.pllM = 1,	//   4MHz /  1 -> 4MHz
			.pllN = 40,	//   4MHz * 40 -> 160MHz <= 344MHz = PLL VCO output max, >= 64 MHz = PLL VCO out min
			.pllR = 2,	// 160MHz /  2 -> 80MHz = F_cpu
		};
		Rcc::enablePll(Rcc::PllSource::MultiSpeedInternalClock, pllFactors);
		Rcc::setFlashLatency<Frequency>();

		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		// APB1 has max. 80MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

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
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_l432kc
/// @{
using Rx = GpioInputA15;
using Tx = GpioOutputA2;
using Uart = Usart2;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_l432kc
/// @{
using Sda = D4;
using Scl = D5;
using Controller = I2cMaster1;
/// @}
}

namespace spi
{
/// @ingroup modm_board_nucleo_l432kc
/// @{
using Cs = D10;
using Sck = D13;
using Sdi = D12;
using Sdo = D11;
using Controller = SpiMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_l432kc
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>(Gpio::InputType::PullUp);
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
	spi::Controller::initialize<SystemClock, 5_MHz>();
}
/// @}

}

#endif	// MODM_STM32_NUCLEO_L432KC_HPP

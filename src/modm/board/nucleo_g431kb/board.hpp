/*
 * Copyright (c) 2020, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_STM32_NUCLEO_G431KB_HPP
#define MODM_STM32_NUCLEO_G431KB_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_g431kb
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_g431kb
/// @{
using namespace modm::literals;

/// STM32G431KB running at 170MHz generated from the internal 16MHz crystal
struct SystemClock
{
	static constexpr uint32_t Frequency = 170_MHz;
	static constexpr uint32_t Ahb1      = Frequency;
	static constexpr uint32_t Ahb2      = Frequency;
	static constexpr uint32_t Apb1      = Frequency;
	static constexpr uint32_t Apb2      = Frequency;

	static constexpr uint32_t Cordic    = Ahb1;
	static constexpr uint32_t Crc       = Ahb1;
	static constexpr uint32_t Dma       = Ahb1;
	static constexpr uint32_t Dma1      = Dma;
	static constexpr uint32_t Dma2      = Dma;
	static constexpr uint32_t DmaMux    = Dma;
	static constexpr uint32_t Fmac      = Ahb1;

	static constexpr uint32_t Adc       = Ahb2;
	static constexpr uint32_t Adc1      = Adc;
	static constexpr uint32_t Adc2      = Adc;
	static constexpr uint32_t Dac       = Ahb2;
	static constexpr uint32_t Dac1      = Dac;
	static constexpr uint32_t Dac3      = Dac;
	static constexpr uint32_t Rng       = Ahb2;

	static constexpr uint32_t Can       = Apb1;
	static constexpr uint32_t Fdcan1    = Can;
	static constexpr uint32_t I2c       = Apb1;
	static constexpr uint32_t I2c1      = I2c;
	static constexpr uint32_t I2c2      = I2c;
	static constexpr uint32_t I2c3      = I2c;
	static constexpr uint32_t I2c4      = I2c;
	static constexpr uint32_t Lptim     = Apb1;
	static constexpr uint32_t Lpuart    = Apb1;
	static constexpr uint32_t Rtc       = Apb1;
	static constexpr uint32_t Spi2      = Apb1;
	static constexpr uint32_t Spi3      = Apb1;
	static constexpr uint32_t Uart4     = Apb1;
	static constexpr uint32_t Uart5     = Apb1;
	static constexpr uint32_t Usart2    = Apb1;
	static constexpr uint32_t Usb       = Apb1;
	static constexpr uint32_t Apb1Timer = Apb1 * 1;
	static constexpr uint32_t Timer2    = Apb1Timer;
	static constexpr uint32_t Timer3    = Apb1Timer;
	static constexpr uint32_t Timer4    = Apb1Timer;
	static constexpr uint32_t Timer5    = Apb1Timer;
	static constexpr uint32_t Timer6    = Apb1Timer;
	static constexpr uint32_t Timer7    = Apb1Timer;

	static constexpr uint32_t Sai1      = Apb2;
	static constexpr uint32_t Spi1      = Apb2;
	static constexpr uint32_t Usart1    = Apb2;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
	static constexpr uint32_t Timer1    = Apb2Timer;
	static constexpr uint32_t Timer8    = Apb2Timer;
	static constexpr uint32_t Timer15   = Apb2Timer;
	static constexpr uint32_t Timer16   = Apb2Timer;
	static constexpr uint32_t Timer17   = Apb2Timer;
	static constexpr uint32_t Timer20   = Apb2Timer;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableInternalClock();	// 16MHz
		Rcc::PllFactors pllFactors{
			.pllM = 4,	//  16MHz / M= 4 ->   4MHz
			.pllN = 85,	//   4MHz * N=85 -> 340MHz
			.pllR = 2,	// 340MHz / R= 2 -> 170MHz = F_cpu
		};
		Rcc::enablePll(Rcc::PllSource::InternalClock, pllFactors);
		Rcc::setFlashLatency<Frequency>();
		Rcc::setVoltageScaling(Rcc::VoltageScaling::Boost); // recommended for >150 MHz
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		// APB1 has max. 170MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

// Arduino Footprint
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
using D5  = GpioA15;
using D6  = GpioB6;
using D7  = GpioF0;
using D8  = GpioF1;
using D9  = GpioA8;
using D10 = GpioA11;
using D11 = GpioB5;
using D12 = GpioB4;
using D13 = GpioB3;

using Button = GpioUnused;
using LedUser = GpioOutputB8;
using Leds = SoftwareGpioPort< LedUser >;
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_g431kb
/// @{
using Rx = GpioInputA3;
using Tx = GpioOutputA2;
using Uart = Usart2;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_g431kb
/// @{
using Sda = D4;
using Scl = D5;
using Controller = I2cMaster1;
/// @}
}

namespace spi
{
/// @ingroup modm_board_nucleo_g431kb
/// @{
using Cs = D10;
using Sck = D13;
using Sdi = D12;
using Sdo = D11;
using Controller = SpiMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_g431kb
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	LedUser::setOutput(modm::Gpio::Low);
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
	spi::Controller::initialize<SystemClock, 5.3125_MHz>();
}
/// @}

}

#endif	// MODM_STM32_NUCLEO_G431KB_HPP

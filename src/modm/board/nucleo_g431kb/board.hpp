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

/// STM32G431KB running at 170MHz generated from the internal 16MHz oscillator
struct SystemClock
{
	static constexpr Rcc::PllConfig pll
	{
		.M = 4,  //  16 MHz /  4 =   4 MHz
		.N = 85, //   4 MHz * 85 = 340 MHz
		.R = 2,  // 340 MHz /  2 = 170 MHz = F_cpu
	};
	static constexpr uint32_t PllR = Rcc::HsiFrequency / pll.M * pll.N / pll.R;
	static_assert(PllR == Rcc::MaxFrequency);

	static constexpr uint32_t Frequency = PllR;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Cordic = Ahb1;
	static constexpr uint32_t Crc = Ahb1;
	static constexpr uint32_t Dma1 = Ahb1;
	static constexpr uint32_t Dma2 = Ahb1;
	static constexpr uint32_t Dmamux1 = Ahb1;
	static constexpr uint32_t Flash = Ahb1;
	static constexpr uint32_t Fmac = Ahb1;

	static constexpr uint32_t Adc1 = Ahb2;
	static constexpr uint32_t Adc2 = Ahb2;
	static constexpr uint32_t Adc12Common = Ahb2;
	static constexpr uint32_t Dac = Ahb2;
	static constexpr uint32_t Dac1 = Ahb2;
	static constexpr uint32_t Dac3 = Ahb2;
	static constexpr uint32_t Rng = Ahb2;

	static constexpr uint32_t Crs = Apb1;
	static constexpr uint32_t Fdcan1 = Apb1;
	static constexpr uint32_t FdcanConfig = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Lptim1 = Apb1;
	static constexpr uint32_t Lpuart1 = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t RtcBus = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Tamp = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Ucpd1 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Usb = Apb1;
	static constexpr uint32_t Wwdg = Apb1;

	static constexpr uint32_t Comp1 = Apb2;
	static constexpr uint32_t Comp2 = Apb2;
	static constexpr uint32_t Comp3 = Apb2;
	static constexpr uint32_t Comp4 = Apb2;
	static constexpr uint32_t Exti = Apb2;
	static constexpr uint32_t Opamp = Apb2;
	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Syscfg = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Vrefbuf = Apb2;

	static constexpr uint32_t Apb1Timer = Apb1;
	static constexpr uint32_t Apb2Timer = Apb2;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;

	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableHsiClock();

		// Required for running above 150 MHz
		Rcc::setVoltageScaling(Rcc::VoltageScaling::Boost);
		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		// The AHB must be divided by 2 for 1us when switching to above 80 MHz in boost mode
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div2);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);

		Rcc::enablePll(Rcc::PllSource::Hsi, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		modm::delay_us(1);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);

		Rcc::setCanClockSource(Rcc::CanClockSource::Pclk);

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
using Uart = BufferedUart<UsartHal2, UartTxBuffer<2048>>;
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

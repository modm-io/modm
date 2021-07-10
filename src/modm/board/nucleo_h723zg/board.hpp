/*
 * Copyright (c) 2021, Christopher Durand
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

using namespace modm::platform;

/// @ingroup modm_board_nucleo_h723zg
/// @{
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
using namespace modm::literals;

/// STM32H723ZG running at 500MHz from PLL clock generated from 8 MHz HSE
struct SystemClock
{
	// Max 550MHz
	static constexpr uint32_t SysClk = 550_MHz;
	// Max 550MHz
	static constexpr uint32_t Hclk = SysClk / 1; // D1CPRE
	static constexpr uint32_t Frequency = Hclk;
	// Max 275MHz
	static constexpr uint32_t Ahb = Hclk / 2; // HPRE
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;
	static constexpr uint32_t Ahb3 = Ahb;
	static constexpr uint32_t Ahb4 = Ahb;
	// Max 137.5MHz
	static constexpr uint32_t Apb1 = Ahb / 2; // D2PPRE1
	static constexpr uint32_t Apb2 = Ahb / 2; // D2PPRE2
	static constexpr uint32_t Apb3 = Ahb / 2; // D1PPRE
	static constexpr uint32_t Apb4 = Ahb / 2; // D3PPRE

	static constexpr uint32_t Adc1 = Ahb1;
	static constexpr uint32_t Adc2 = Ahb1;
	static constexpr uint32_t Adc3 = Ahb4;

	static constexpr uint32_t Dac1 = Apb1;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Spi6 = Apb4;

	static constexpr uint32_t Usart1  = Apb2;
	static constexpr uint32_t Usart2  = Apb1;
	static constexpr uint32_t Usart3  = Apb1;
	static constexpr uint32_t Uart4   = Apb1;
	static constexpr uint32_t Uart5   = Apb1;
	static constexpr uint32_t Usart6  = Apb2;
	static constexpr uint32_t Uart7   = Apb1;
	static constexpr uint32_t Uart8   = Apb1;
	static constexpr uint32_t Uart9   = Apb2;
	static constexpr uint32_t Usart10 = Apb2;

	static constexpr uint32_t LpUart1 = Apb4;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb4;
	static constexpr uint32_t I2c5 = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 2;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;
	static constexpr uint32_t Timer23 = Apb1Timer;
	static constexpr uint32_t Timer24 = Apb1Timer;

	static constexpr uint32_t Usb = Ahb1;

	static bool inline
	enable()
	{
		// Switch core supply voltage to maximum level
		// Required for running at 550 MHz
		Rcc::setVoltageScaling(Rcc::VoltageScaling::Scale0);

		Rcc::enableExternalClock(); // 8 MHz
		const Rcc::PllFactors pllFactors1{
			.range = Rcc::PllInputRange::MHz1_2,
			.pllM  = 4,		//   8 MHz / 4   =   2 MHz
			.pllN  = 275,	//   2 MHz * 275 = 550 MHz
			.pllP  = 1,		// 500 MHz / 1   = 550 MHz
			.pllQ  = 2,		// 500 MHz / 2   = 275 MHz
			.pllR  = 2,		// 500 MHz / 2   = 275 MHz
		};
		Rcc::enablePll1(Rcc::PllSource::Hse, pllFactors1);
		Rcc::setFlashLatency<Ahb>();

		// max. 275MHz
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div2);
		// max. 137.5MHz on Apb clocks
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div2);
		Rcc::setApb3Prescaler(Rcc::Apb3Prescaler::Div2);
		Rcc::setApb4Prescaler(Rcc::Apb4Prescaler::Div2);

		// update clock frequencies
		Rcc::updateCoreFrequency<Frequency>();
		// switch system clock to pll
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1P);

		return true;
	}

};

// Arduino Footprint
#include "nucleo144_arduino.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputB0;
using LedYellow = GpioOutputE1;
using LedRed = GpioOutputB14;
using Leds = SoftwareGpioPort< LedRed, LedYellow, LedGreen >;

namespace stlink
{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = Usart3;
}

using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;


inline void
initialize()
{
    SystemClock::enable();
    SysTickTimer::initialize<SystemClock>();

    stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
    stlink::Uart::initialize<SystemClock, 115200_Bd>();

    LedGreen::setOutput(modm::Gpio::Low);
    LedYellow::setOutput(modm::Gpio::Low);
    LedRed::setOutput(modm::Gpio::Low);

    Button::setInput();
    Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
    Button::enableExternalInterrupt();
//  Button::enableExternalInterruptVector(12);
}

}
/// @}


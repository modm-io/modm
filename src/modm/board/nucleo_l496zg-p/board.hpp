// coding: utf-8
/*
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_NUCLEO_L496ZG_P_HPP
#define MODM_STM32_NUCLEO_L496ZG_P_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

/// @ingroup modm_board_nucleo_l496zg_p
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_nucleo_l496zg_p
namespace Board
{
	using namespace modm::literals;

/// STM32L4 running at 80MHz generated from the
/// internal Multispeed oscillator

// Dummy clock for devices
struct SystemClock {
	static constexpr uint32_t Frequency = 80_MHz;
	static constexpr uint32_t Ahb  = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;
	static constexpr uint32_t Apb1Timer = Apb1;
	static constexpr uint32_t Apb2Timer = Apb2;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;

	static constexpr uint32_t Comp1 = Apb2;
	static constexpr uint32_t Comp2 = Apb2;

	static constexpr uint32_t Dac1 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;

	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t Sai2 = Apb2;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;

	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;

	static constexpr uint32_t Usart1  = Apb2;
	static constexpr uint32_t Usart2  = Apb1;
	static constexpr uint32_t Usart3  = Apb1;
	static constexpr uint32_t Uart4   = Apb1;
	static constexpr uint32_t Uart5   = Apb1;

	static constexpr uint32_t Lpuart1 = Frequency; // Clock mux default: 0b00 -> PCLK

	static constexpr uint32_t Usb = 48_MHz;
	static constexpr uint32_t Rng = 48_MHz;
	static constexpr uint32_t Sdmmc = 48_MHz;

	static bool inline
	enable()
	{
		Rcc::enableMultiSpeedInternalClock(Rcc::MsiFrequency::MHz16);
		const Rcc::PllFactors pllFactors{
			.pllM = 2,		//   16MHz / M=2  ->   8MHz
			.pllN = 40,		//   8MHz *  N=40 -> 320MHz
			.pllR = 4,		// 320MHz /  R=4  ->  80MHz = F_cpu
			.pllQ = 2,		// 320MHz /  Q=2  -> 160MHz
		};
		Rcc::enablePll(Rcc::PllSource::Msi, pllFactors);
		Rcc::setFlashLatency<Frequency>();
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update clock frequencies
		Rcc::updateCoreFrequency<Frequency>();

		// Enable Hsi48 clock
		uint32_t waitCycles = 2048;
		RCC->CRRCR |= RCC_CRRCR_HSI48ON;
		while (not (RCC->CRRCR & RCC_CRRCR_HSI48RDY) and --waitCycles)
			;

		// Select Hsi48 as source for CLK48 MUX (USB, SDMMC, RNG)
		RCC->CCIPR = (RCC->CCIPR & ~RCC_CCIPR_CLK48SEL_Msk) | (0b00 << RCC_CCIPR_CLK48SEL_Pos);

		return true;
	}
};

// Arduino Footprint
#include "nucleo144_arduino_l4.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputC7;	// LED1 [Green]
using LedBlue = GpioOutputB7;	// LED2 [Blue]
using LedRed = GpioOutputB14;	// LED3 [Red]
using Leds = SoftwareGpioPort< LedRed, LedBlue, LedGreen >;

namespace usb
{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Overcurrent = GpioInputG5;	// OTG_FS_OverCurrent
using Power = GpioOutputG6;			// OTG_FS_PowerSwitchOn

using Device = UsbFs;
}

namespace stlink
{
using Rx = GpioOutputG8;
using Tx = GpioInputG7;
using Uart = Lpuart1;
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
	LedBlue::setOutput(modm::Gpio::Low);
	LedRed::setOutput(modm::Gpio::Low);

	Button::setInput();
//	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
//	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

inline void
initializeUsbFs()
{
	usb::Device::initialize<SystemClock>();
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();
}

} // Board namespace

#endif	// MODM_STM32_NUCLEO_L496ZG_P_HPP

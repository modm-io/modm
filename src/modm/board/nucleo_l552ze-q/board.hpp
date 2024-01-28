// coding: utf-8
/*
 * Copyright (c) 2021, Raphael Lehmann
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_NUCLEO_L552ZE_Q_HPP
#define MODM_STM32_NUCLEO_L552ZE_Q_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

/// @ingroup modm_board_nucleo_l552ze_q
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using namespace modm::literals;

/// STM32L5 running at 110 MHz generated from the
/// internal Multispeed oscillator
struct SystemClock
{
	static constexpr uint32_t Frequency = 110_MHz;
	static constexpr uint32_t Ahb  = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;
	static constexpr uint32_t Apb1Timer = Apb1;
	static constexpr uint32_t Apb2Timer = Apb2;

	static constexpr uint32_t Can1 = Apb1;

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
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::setVoltageScaling(Rcc::VoltageScaling::Range0);
		Rcc::enableMultiSpeedInternalClock(Rcc::MsiFrequency::MHz4);
		const Rcc::PllFactors pllFactors{
			.pllM = 1,		//   4MHz /  M=1  ->   4MHz
			.pllN = 55,		//   4MHz *  N=55 -> 220MHz
			.pllR = 2,		// 220MHz /  R=2  -> 110MHz = F_cpu
			.pllQ = 2,		// 220MHz /  Q=2  -> 110MHz
		};
		Rcc::enablePll(Rcc::PllSource::Msi, pllFactors);
		Rcc::setFlashLatency<Frequency>();
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update clock frequencies
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enableInternalClockMHz48();
		// Select Hsi48 as source for CLK48 MUX (USB, SDMMC, RNG)
		Rcc::setClock48Source(Rcc::Clock48Source::Hsi48);

		return true;
	}
};

// Arduino Footprint
#include "nucleo144_arduino_l5.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputC7;	// LED1 [Green]
using LedBlue = GpioOutputB7;	// LED2 [Blue]
using LedRed = GpioOutputA9;	// LED3 [Red]
using Leds = SoftwareGpioPort<LedRed, LedBlue, LedGreen>;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using Dm = GpioA11;
using Dp = GpioA12;

using UcpdFlt = GpioB14;
using UcpdDBn = GpioB5;
using UcpdCc1 = GpioA15;
using UcpdCc2 = GpioB15;

using Device = UsbFs;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using Rx = GpioOutputG8;
using Tx = GpioInputG7;
using Uart = Lpuart1;
/// @}
}

/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using LoggerDevice = modm::IODeviceWrapper<stlink::Uart, modm::IOBuffer::BlockIfFull>;

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
}

inline void
initializeUsbFs(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}
/// @}

} // Board namespace

#endif	// MODM_STM32_NUCLEO_L552ZE_Q_HPP

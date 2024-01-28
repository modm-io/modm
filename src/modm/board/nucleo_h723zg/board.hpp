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
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_h723zg
/// @{
using namespace modm::literals;

/// STM32H723ZG running at 550MHz from PLL clock generated from 8 MHz HSE
struct SystemClock
{
	// Max 550MHz
	static constexpr uint32_t SysClk = 550_MHz;
	static constexpr uint32_t Pll1Q = SysClk / 4;
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

	static constexpr uint32_t Spi1 = Pll1Q;
	static constexpr uint32_t Spi2 = Pll1Q;
	static constexpr uint32_t Spi3 = Pll1Q;
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

	static constexpr uint32_t Usb = 48_MHz; // From PLL3Q
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

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
			.pllP  = 1,		// 550 MHz / 1   = 550 MHz
			.pllQ  = 4,		// 550 MHz / 4   = 137.5 MHz
			.pllR  = 2,		// 550 MHz / 2   = 275 MHz
		};
		Rcc::enablePll1(Rcc::PllSource::Hse, pllFactors1);
		// Use PLL3 for USB 48MHz
		const Rcc::PllFactors pllFactors3{
			.range = Rcc::PllInputRange::MHz4_8,
			.pllM = 2,		//   8MHz / M=   4MHz
			.pllN = 60,		//   4MHz * N= 240MHz
			.pllP = 5,		// 240MHz / P=  48MHz
			.pllQ = 5,		// 240MHz / Q=  48MHz = F_usb
			.pllR = 5,		// 240MHz / R=  48MHz
		};
		Rcc::enablePll3(Rcc::PllSource::ExternalClock, pllFactors3);
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
		Rcc::enableUsbClockSource(Rcc::UsbClockSource::Pll3Q);
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
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_h723zg
/// @{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Overcurrent = GpioInputG7;
using Power = GpioOutputG6;

using Device = UsbHs;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_h723zg
/// @{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = Usart3;
/// @}
}

/// @ingroup modm_board_nucleo_h723zg
/// @{
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
}

inline void
initializeUsbFs(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();
	// Enable VBUS sense (B device) via pin PA9
	USB_OTG_HS->GCCFG |= USB_OTG_GCCFG_VBDEN;
}
/// @}

}


/*
 * Copyright (c) 2021-2022, Christopher Durand
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

/// @ingroup modm_board_nucleo_h755zi-q
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_h755zi-q
/// @{
using namespace modm::literals;

/// STM32H755 running at 480MHz from the external 8MHz HSE
struct SystemClock
{
	static constexpr uint32_t SysClk = 480_MHz;

	// Max 480MHz
	static constexpr uint32_t Hclk = SysClk / 1; // D1CPRE
	// Max 240MHz
	static constexpr uint32_t Ahb = Hclk / 2; // HPRE

#ifdef CORE_CM7
	static constexpr uint32_t Frequency = Hclk;
#else
	static constexpr uint32_t Frequency = Ahb;
#endif

	// Max 240MHz
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;
	static constexpr uint32_t Ahb3 = Ahb;
	static constexpr uint32_t Ahb4 = Ahb;
	// Max 120MHz
	static constexpr uint32_t Apb1 = Ahb / 2; // D2PPRE1
	static constexpr uint32_t Apb2 = Ahb / 2; // D2PPRE2
	static constexpr uint32_t Apb3 = Ahb / 2; // D1PPRE
	static constexpr uint32_t Apb4 = Ahb / 2; // D3PPRE

	static constexpr uint32_t Adc1 = Ahb1;
	static constexpr uint32_t Adc2 = Ahb1;
	static constexpr uint32_t Adc3 = Ahb4;

	static constexpr uint32_t Dac1 = Apb1;
	static constexpr uint32_t Dac2 = Apb1;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Spi6 = Apb4;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;
	static constexpr uint32_t Usart6 = Apb2;
	static constexpr uint32_t Uart7  = Apb1;
	static constexpr uint32_t Uart8  = Apb1;

	static constexpr uint32_t LpUart1 = Apb4;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb4;

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

	static constexpr uint32_t Usb = 48_MHz;

#ifdef CORE_CM7
	static bool inline
	enable()
	{
		Rcc::enableExternalClock(); // 8 MHz
		Rcc::setVoltageScaling(Rcc::VoltageScaling::Scale0); // required for 480MHz
		const Rcc::PllFactors pllFactors {
			.range = Rcc::PllInputRange::MHz4_8,
			.pllM = 1,		//   8MHz / M=   8MHz
			.pllN = 120,	//   8MHz * N= 960MHz
			.pllP = 2,		// 960MHz / P= 480MHz = F_cpu
			.pllQ = 20,		// 960MHz / Q=  48MHz
			.pllR = 2,		// 960MHz / R= 480MHz
		};
		Rcc::enablePll1(Rcc::PllSource::ExternalClock, pllFactors);
		Rcc::setFlashLatency<Ahb>();
		// max 240MHz on AHB
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div2);
		// max 120MHz on APB
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div2);
		Rcc::setApb3Prescaler(Rcc::Apb3Prescaler::Div2);
		Rcc::setApb4Prescaler(Rcc::Apb4Prescaler::Div2);
		// update clock frequencies
		Rcc::updateCoreFrequency<Frequency>();
		Rcc::enableUsbClockSource(Rcc::UsbClockSource::Pll1Q);
		// Switch the main clock source to PLL
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1P);
		return true;
	}
#else
	static bool inline
	enable()
	{
		Rcc::updateCoreFrequency<Frequency>();
		return true;
	}
#endif
};

// Arduino Footprint
#include "nucleo144_arduino_h745_55.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputB0;
using LedYellow = GpioOutputE1;
using LedRed = GpioOutputB14;
using Leds = SoftwareGpioPort<LedRed, LedYellow, LedGreen>;

namespace usb
{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Overcurrent = GpioInputG7;	// OTG_FS_OverCurrent
using Power = GpioOutputD10;			// OTG_FS_PowerSwitchOn

using Device = UsbFs;
}

namespace stlink
{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = Usart3;
}

#ifdef CORE_CM7
using LoggerDevice = modm::IODeviceWrapper<stlink::Uart, modm::IOBuffer::BlockIfFull>;
#endif

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

#ifdef CORE_CM7
	Rcc::bootCortexM4();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedYellow::setOutput(modm::Gpio::Low);
	LedRed::setOutput(modm::Gpio::Low);

	Button::setInput();
#endif
}

inline void
initializeUsbFs(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();
	usb::Id::configure(Gpio::InputType::Floating);

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();
	// Enable VBUS sense (B device) via pin PA9
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBDEN;
}

}
/// @}


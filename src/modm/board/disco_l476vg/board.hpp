// coding: utf-8
/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2017-2018, 2021, Niklas Hauser
 * Copyright (c) 2018, Antal Szabó
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_L476_DISCOVERY_HPP
#define MODM_STM32_L476_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

/// @ingroup modm_board_disco_l476vg
namespace Board
{
using namespace modm::literals;

/// STM32L4 running at 80MHz generated from the
/// Internal Multispeed Oscillator
struct SystemClock
{
	static constexpr uint32_t Frequency = 80_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Usb = 48_MHz;

	static constexpr uint32_t Usart1 = Apb2;

	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Usart4 = Apb1;
	static constexpr uint32_t Usart5 = Apb1;

	static bool inline
	enable()
	{
		// set flash latency first because system already runs from MSI
		Rcc::setFlashLatency<Frequency>();

		Rcc::enableLowSpeedExternalCrystal();
		// // Disable the LSE Clock Security System
		RCC->BDCR &= ~RCC_BDCR_LSECSSON;

		Rcc::enableMultiSpeedInternalClock(Rcc::MsiFrequency::MHz48);

		const Rcc::PllFactors pllFactors{
			.pllM = 6,	// 48MHz / M=6 -> 8MHz
			.pllN = 40,	// 8MHz * N=40 -> 320MHz <= 344MHz = PLL VCO output max, >= 64 MHz = PLL VCO out min
			.pllR = 4,	// 320MHz / R=4 -> 80MHz = F_cpu
			.pllQ = 4	// 320MHz / Q=4 -> 80MHz
		};
		Rcc::enablePll(Rcc::PllSource::MultiSpeedInternalClock, pllFactors);
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		// all max. 80MHz
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		// Enable MSI Auto-calibration through LSE
		RCC->CR |= RCC_CR_MSIPLLEN;

		// Choose MSI48 for USB
		RCC->CCIPR |= RCC_CCIPR_CLK48SEL;

		return true;
	}
};

using Button = GpioInputA0;

namespace Joystick
{
using Left  = GpioInputA1;
using Right = GpioInputA2;
using Up    = GpioInputA3;
using Down  = GpioInputA5;
}

using LedRed   = GpioOutputB2;	// User LED 4
using LedGreen = GpioOutputE8;	// User LED 5

using Leds = SoftwareGpioPort< LedRed, LedGreen >;

namespace usb
{
using Overcurrent = GpioInputC10;	// OTG_FS_OverCurrent
using Vbus = GpioInputA9; // default PC11 if SB24 open
using Id = GpioA10; // default PC12 if SB25 left open
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
}

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedRed::setOutput(modm::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

/// You must take out the LCD screen and close SB24 and SB25 for USB to work
inline void
initializeUsbFs()
{
	usb::Device::initialize<SystemClock>();
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();

	// Enable VBUS sense (B device) via pin PA9
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBDEN;
}

}

#endif	// MODM_STM32_L476_DISCOVERY_HPP

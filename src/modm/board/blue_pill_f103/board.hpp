/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F103C8T6_BLUE_PILL_HPP
#define MODM_STM32_F103C8T6_BLUE_PILL_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;


/// @ingroup modm_board_blue_pill_f103
/// @{
namespace Board
{
using namespace modm::literals;

/// STM32F103 running at 72MHz generated from the external 8MHz crystal
struct SystemClock
{
	static constexpr uint32_t Frequency = 72_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 2;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc  = Apb2;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;

	static constexpr uint32_t Can    = Apb1;

	static constexpr uint32_t I2c1   = Apb1;
	static constexpr uint32_t I2c2   = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;

	static constexpr uint32_t Usb = Ahb / 1.5;

	static bool inline
	enable()
	{
		Rcc::enableExternalCrystal();

		// external clock * 9 = 72MHz, => 72/1.5 = 48 => good for USB
		const Rcc::PllFactors pllFactors{
			.pllMul = 9,
			.usbPrediv = Rcc::UsbPrescaler::Div1_5
		};
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, pllFactors);

		// set flash latency for 72MHz
		Rcc::setFlashLatency<Frequency>();

		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);

		// AHB has max 72MHz
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);

		// APB1 has max. 36MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div2);

		// APB2 has max. 72MHz
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);

		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

namespace usb
{
using Dm = GpioA11;		// DM: USB_DM
using Dp = GpioA12;		// DP: USB_DP
using Device = UsbFs;
}

// User LED (inverted, because connected to 3V3)
using LedGreen = GpioInverted< GpioOutputC13 >;
using Leds = SoftwareGpioPort< LedGreen >;

using Button = GpioUnused;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen::setOutput(modm::Gpio::Low);
}

inline void
initializeUsbFs()
{
	usb::Device::initialize<SystemClock>();
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}

} // Board namespace
/// @}

#endif	// MODM_STM32_F103C8T6_BLUE_PILL_HPP

/*
 * Copyright (c) 2019, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F4X1CEU_MINI_F4X1_HPP
#define MODM_STM32_F4X1CEU_MINI_F4X1_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;


/// @ingroup modm_board_black_pill_f401 modm_board_black_pill_f411
/// @{
namespace Board
{
using namespace modm::literals;

/// STM32F4x1 running at 84MHz generated from the external 25MHz crystal
struct SystemClock
{
	static constexpr uint32_t Frequency = 84_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 2;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc  = Apb2;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart6 = Apb2;

	static constexpr uint32_t I2c1   = Apb1;
	static constexpr uint32_t I2c2   = Apb1;
	static constexpr uint32_t I2c3   = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer9  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;

	static constexpr uint32_t Usb = 48_MHz;

	static bool inline
	enable()
	{
		Rcc::enableExternalCrystal();
		const Rcc::PllFactors pllFactors{
			.pllM = 25,		// 25MHz / M=25 -> 1MHz
			.pllN = 336,	// 1MHz * N=336 -> 336MHz
			.pllP = 4,		// 336MHz / P=4 -> 84MHz = F_cpu
			.pllQ = 7,		// 336MHz / Q=7 -> 48MHz = F_usb
		};
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, pllFactors);

		// set flash latency
		Rcc::setFlashLatency<Frequency>();

		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);

		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

namespace usb
{
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
}

// User LED (inverted, because connected to 3V3)
using Led = GpioInverted< GpioOutputC13 >;
using Leds = SoftwareGpioPort< Led >;

using Button = GpioInverted< GpioInputA0 >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Led::setOutput(modm::Gpio::Low);
	Button::setInput(Gpio::InputType::PullUp);
}

inline void
initializeUsbFs()
{
	usb::Device::initialize<SystemClock>();
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();
	usb::Id::configure(Gpio::InputType::PullUp);

	// explicitly disable VBUS sense (B device)
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
	USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBUSBSEN;
	USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBUSASEN;
}

} // Board namespace
/// @}

#endif	// MODM_STM32_F4X1CEU_MINI_F4X1_HPP

// coding: utf-8
/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
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
// #include <modm/driver/inertial/lis3dsh.hpp>

using namespace modm::platform;

/// @ingroup modm_board_disco_l476vg
namespace Board
{
	using namespace modm::literals;

/// STM32L4 running at 48MHz generated from the
/// internal Multispeed oscillator

// Dummy clock for devices
struct SystemClock {
	static constexpr uint32_t Frequency = 48_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

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

		Rcc::enableMultiSpeedInternalClock(Rcc::MsiFrequency::MHz48);

		// Rcc::enablePll(
		// 	Rcc::PllSource::MultiSpeedInternalClock,
		// 	1,	// 4MHz / N=1 -> 4MHz
		// 	16,	// 4MHz * M=16 -> 64MHz <= 344MHz = PLL VCO output max, >= 64 MHz = PLL VCO out min
		// 	1,	// 64MHz / P=1 -> 64MHz = F_cpu
		// 	2	// 64MHz / Q=2 -> 32MHz = F_usb
		// );
		// switch system clock to PLL output
		// Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		// Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		// APB1 has max. 50MHz
		// Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div2);
		// Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

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

}

#endif	// MODM_STM32_L476_DISCOVERY_HPP

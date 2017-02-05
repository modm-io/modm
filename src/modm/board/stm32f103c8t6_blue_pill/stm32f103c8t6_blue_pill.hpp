/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

//
// STM32F103C8T6 "Blue Pill" Minimum System Development Board
//
// Cheap and bread-board-friendly board for STM32 F1 series.
// Sold for less than 2 USD on well known Internet shops from China.
//
// http://wiki.stm32duino.com/index.php?title=Blue_Pill
//

#ifndef MODM_STM32_F103C8T6_BLUE_PILL_HPP
#define MODM_STM32_F103C8T6_BLUE_PILL_HPP

#include <modm/platform/platform.hpp>

using namespace modm::platform;


namespace Board
{

/// STM32F103 running at 72MHz generated from the external 8MHz crystal
// Dummy clock for devices
struct systemClock {
	static constexpr uint32_t Frequency = MHz72;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 2;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc1 = Apb2;
	static constexpr uint32_t Adc2 = Apb2;
	static constexpr uint32_t Adc3 = Apb2;

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

	static bool inline
	enable()
	{
		ClockControl::enableExternalCrystal();

		// external clock * 9 = 72MHz, => 72/1.5 = 48 => good for USB
		ClockControl::enablePll(ClockControl::PllSource::ExternalCrystal, ClockControl::UsbPrescaler::Div1_5, 9);

		// set flash latency for 72MHz
		ClockControl::setFlashLatency(Frequency);

		// switch system clock to PLL output
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);

		// AHB has max 72MHz
		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);

		// APB1 has max. 36MHz
		ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div2);

		// APB2 has max. 72MHz
		ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions


		// update frequencies for busy-wait delay functions
		modm::clock::fcpu     = Frequency;
		modm::clock::fcpu_kHz = Frequency / 1000;
		modm::clock::fcpu_MHz = Frequency / 1000000;
		modm::clock::ns_per_loop = ::round(3000.f / (Frequency / 1000000));

		return true;
	}
};

// Not using SystemClock as it is not yet finished for F1 series.
// But this works by accident.
// using systemClock = SystemClock<Pll<ExternalCrystal<MHz8>, MHz72> >;

// User LED (inverted, because connected to 3V3)
using LedGreen = GpioInverted< GpioOutputC13 >;
using Leds = SoftwareGpioPort< LedGreen >;

using Button = GpioUnused;

inline void
initialize()
{
	systemClock::enable();
	modm::cortex::SysTickTimer::initialize<systemClock>();

	LedGreen::setOutput(modm::Gpio::Low);
}

} // Board namespace

#endif	// MODM_STM32_F103C8T6_BLUE_PILL_HPP

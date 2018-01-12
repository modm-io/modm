// coding: utf-8
/* Copyright (c) 2017, Sascha Schade (strongly-typed)
* All Rights Reserved.
*
* The file is part of the modm library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F0DISCOVERY
// Discovery kit for STM32F0 series
// http://www.st.com/en/evaluation-tools/stm32f0discovery.html
//

#ifndef MODM_STM32_F0_DISCOVERY_HPP
#define MODM_STM32_F0_DISCOVERY_HPP

#include <modm/platform/platform.hpp>

using namespace modm::platform;


namespace Board
{

/// STM32F0 running at 48MHz generated from the internal 8MHz with PLL.

struct systemClock
{
	static constexpr int Frequency = MHz48;
	static constexpr int Usart1 = Frequency;
	static constexpr int Usart2 = Frequency;
	static constexpr int Spi2 = Frequency;

	static bool inline
	enable()
	{
		// enable internal 8 MHz HSI RC clock
		ClockControl::enableInternalClock();
		// (internal clock / 2) * 12 = 48MHz
		ClockControl::enablePll(ClockControl::PllSource::InternalClock, 12, 1);
		// set flash latency for 48MHz
		ClockControl::setFlashLatency(Frequency);
		// switch system clock to PLL output
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		ClockControl::setApbPrescaler(ClockControl::ApbPrescaler::Div1);
		// update frequencies for busy-wait delay functions
		modm::clock::fcpu     = Frequency;
		modm::clock::fcpu_kHz = Frequency / 1000;
		modm::clock::fcpu_MHz = Frequency / 1000000;
		modm::clock::ns_per_loop = ::round(4000.f / (Frequency / 1000000));

		return true;
	}
};


using Button   = GpioInputA0;

using LedGreen = GpioOutputC9;
using LedBlue  = GpioOutputC8;

using Leds = SoftwareGpioPort< LedGreen, LedBlue >;


inline void
initialize()
{
	systemClock::enable();
	modm::cortex::SysTickTimer::initialize<systemClock>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedBlue::setOutput(modm::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

} // namespace Board

#endif	// MODM_STM32_F0_DISCOVERY_HPP

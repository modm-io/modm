// coding: utf-8
/* Copyright (c) 2017, Sascha Schade (strongly-typed)
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32L476DISCOVERY
// Discovery kit for STM32L476
// http://www.st.com/en/evaluation-tools/32l476gdiscovery.html
//

#ifndef XPCC_STM32_L476_DISCOVERY_HPP
#define XPCC_STM32_L476_DISCOVERY_HPP

#include <xpcc/architecture/platform.hpp>
// #include <xpcc/driver/inertial/lis3dsh.hpp>

using namespace xpcc::stm32;


namespace Board
{

/// STM32L4 running at 48MHz generated from the
/// internal Multispeed oscillator

// Dummy clock for devices
struct systemClock {
	static constexpr uint32_t Frequency = 48 * MHz1;
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
		ClockControl::setFlashLatency(Frequency);

		ClockControl::enableMultiSpeedInternalClock(ClockControl::MsiFrequency::MHz48);

		// ClockControl::enablePll(
		// 	ClockControl::PllSource::MultiSpeedInternalClock,
		// 	1,	// 4MHz / N=1 -> 4MHz
		// 	16,	// 4MHz * M=16 -> 64MHz <= 344MHz = PLL VCO output max, >= 64 MHz = PLL VCO out min
		// 	1,	// 64MHz / P=1 -> 64MHz = F_cpu
		// 	2	// 64MHz / Q=2 -> 32MHz = F_usb
		// );
		// switch system clock to PLL output
		// ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		// ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		// APB1 has max. 50MHz
		// ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div2);
		// ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		xpcc::clock::fcpu     = Frequency;
		xpcc::clock::fcpu_kHz = Frequency / 1000;
		xpcc::clock::fcpu_MHz = Frequency / 1000000;
		xpcc::clock::ns_per_loop = ::round(3000.f / (Frequency / 1000000));

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

using Leds = xpcc::SoftwareGpioPort< LedRed, LedGreen >;


inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

}

#endif	// XPCC_STM32_L476_DISCOVERY_HPP

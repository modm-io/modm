// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F1DISCOVERY
// Discovery kit for STM32 F1 series
// http://www.st.com/web/en/catalog/tools/FM116/SC959/SS1532/PF250863
//

#ifndef XPCC_STM32_F1_DISCOVERY_HPP
#define XPCC_STM32_F1_DISCOVERY_HPP

#include <xpcc/architecture/platform.hpp>

using namespace xpcc::stm32;


namespace Board
{

/// STM32F100 running at 24MHz generated from the external 8MHz crystal
/// supplied by the on-board st-link
using systemClock = SystemClock<Pll<ExternalCrystal<MHz8>, MHz24> >;


using Button = GpioInputA0;	// Blue PushButton

using LedGreen = GpioOutputC9;		// User LED 3
using LedBlue  = GpioOutputC8;		// User LED 4



inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	LedGreen::setOutput(xpcc::Gpio::Low);
	LedBlue::setOutput(xpcc::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

}

#endif	// XPCC_STM32_F1_DISCOVERY_HPP

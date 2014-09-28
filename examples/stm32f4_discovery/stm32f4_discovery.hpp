// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F4DISCOVERY
// Discovery kit for STM32F407/417 lines
// http://www.st.com/web/en/catalog/tools/FM116/SC959/SS1532/PF252419
//

#ifndef XPCC_STM32_F4_DISCOVERY_HPP
#define XPCC_STM32_F4_DISCOVERY_HPP

using namespace xpcc::stm32;

typedef GpioOutputD13 LedOrange;	// User LED 3
typedef GpioOutputD12 LedGreen;		// User LED 4
typedef GpioOutputD14 LedRed;		// User LED 5
typedef GpioOutputD15 LedBlue;		// User LED 6

typedef GpioOutputA9 VBusPresent;		// green LED (LD7)
typedef GpioOutputD5 VBusOvercurrent;	// red LED (LD8)

typedef GpioInputA0 Button;
typedef GpioOutputA8 ClockOut;
typedef GpioOutputC9 SystemClockOut;


/// STM32F4 running at 168MHz (USB Clock qt 48MHz) generated from the
/// external on-board 8MHz crystal
typedef SystemClock<Pll<ExternalCrystal<MHz8>, MHz168, MHz48> > defaultSystemClock;

#endif	// XPCC_STM32_F4_DISCOVERY_HPP

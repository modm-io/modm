// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F4DISCOVERY
// Discovery kit for STM32F429 line
// http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF259090
//

#ifndef XPCC_STM32_F429_DISCOVERY_HPP
#define XPCC_STM32_F429_DISCOVERY_HPP

using namespace xpcc::stm32;

typedef GpioOutputG13 LedGreen;		// User LED (LD3)
typedef GpioOutputG14  LedRed;		// User LED (LD4)

typedef GpioOutputB13 VBusPresent;		// green LED (LD5)
typedef GpioOutputC5  VBusOvercurrent;	// red LED (LD6)

typedef GpioInputA0 Button;


/// STM32F4 running at 168MHz (USB Clock qt 48MHz) generated from the
/// external on-board 8MHz crystal
typedef SystemClock<Pll<ExternalCrystal<MHz8>, MHz168, MHz48> > defaultSystemClock;

#endif	// XPCC_STM32_F429_DISCOVERY_HPP

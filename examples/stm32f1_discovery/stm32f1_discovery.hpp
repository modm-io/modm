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
// www.st.com/stm32-discovery
//

#ifndef XPCC_STM32_F1_DISCOVERY_HPP
#define XPCC_STM32_F1_DISCOVERY_HPP

using namespace xpcc::stm32;

typedef GpioOutputC9  LedLeft;
typedef GpioOutputC8  LedRight;
typedef GpioInputA0   Button;

/// STM32F100 running at 24MHz generated from the external 8MHz crystal
/// supplied by the on-board st-link
typedef SystemClock<Pll<ExternalCrystal<MHz8>, MHz24> > defaultSystemClock;

#endif	// XPCC_STM32_F1_DISCOVERY_HPP

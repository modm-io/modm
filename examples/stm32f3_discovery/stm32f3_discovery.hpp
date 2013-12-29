// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F3DISCOVERY
// Discovery kit for STM32 F3 series
// http://www.st.com/web/en/catalog/tools/FM116/SC959/SS1532/PF254044
//

#ifndef XPCC_STM32_F3_DISCOVERY_HPP
#define XPCC_STM32_F3_DISCOVERY_HPP

using namespace xpcc::stm32;

typedef GpioOutputE9  LedNorth;
typedef GpioOutputE10 LedNorthEast;
typedef GpioOutputE11 LedEast;
typedef GpioOutputE12 LedSouthEast;
typedef GpioOutputE13 LedSouth;
typedef GpioOutputE14 LedSouthWest;
typedef GpioOutputE15 LedWest;
typedef GpioOutputE8  LedNorthWest;
typedef GpioOutputA8  ClockOut;
typedef GpioInputA0   Button;

/// STM32F3 running at 72MHz generated from the external 8MHz clock
/// supplied by the on-board st-link
typedef SystemClock<Pll<ExternalClock<MHz8>, MHz72> > defaultSystemClock;

#endif	// XPCC_STM32_F3_DISCOVERY_HPP

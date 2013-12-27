// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// Loa Board v2b
// STM32F4 and Spartan FPGA
// https://github.com/dergraaf/loa
//

#ifndef XPCC_STM32_F4_LOA_V2B_HPP
#define XPCC_STM32_F4_LOA_V2B_HPP

using namespace xpcc::stm32;

typedef GpioOutputE5 LedWhite;
typedef GpioOutputE6 LedGreen;

/// STM32F4 running at 168MHz (USB Clock qt 48MHz) generated from the
/// external on-board 25MHz oscillator
typedef SystemClock<Pll<ExternalClock<MHz25>, MHz168, MHz48> > defaultSystemClock;

#endif	// XPCC_STM32_F4_LOA_V2B_HPP

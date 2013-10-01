// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32_F4_DISCOVERY_HPP
#define XPCC_STM32_F4_DISCOVERY_HPP

typedef ::xpcc::stm32::GpioOutputD13 LedOrange;	// User LED 3
typedef ::xpcc::stm32::GpioOutputD12 LedGreen;		// User LED 4
typedef ::xpcc::stm32::GpioOutputD14 LedRed;		// User LED 5
typedef ::xpcc::stm32::GpioOutputD15 LedBlue;		// User LED 6

typedef ::xpcc::stm32::GpioOutputA9 VBusPresent;		// green LED (LD7)
typedef ::xpcc::stm32::GpioOutputD5 VBusOvercurrent;	// red LED (LD8)

typedef ::xpcc::stm32::GpioInputA0 Button;
typedef ::xpcc::stm32::GpioOutputA8 ClockOut;
typedef ::xpcc::stm32::GpioOutputC9 SystemClockOut;

#endif	// XPCC_STM32_F4_DISCOVERY_HPP

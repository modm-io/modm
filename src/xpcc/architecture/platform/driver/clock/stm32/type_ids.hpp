// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32_CLOCK_TYPE_IDS_HPP
#define XPCC_STM32_CLOCK_TYPE_IDS_HPP

namespace xpcc
{

namespace stm32
{

namespace TypeId
{
	typedef struct{} InternalClock;
	typedef struct{} ExternalClock;
	typedef struct{} ExternalCrystal;
	typedef struct{} Pll;
	typedef struct{} SystemClock;
	typedef struct{} ClockOutput;
	typedef struct{} ClockOutput1;
	typedef struct{} ClockOutput2;
}

}   // namespace stm32

}   // namespace xpcc

#endif // XPCC_STM32_CLOCK_TYPE_IDS_HPP

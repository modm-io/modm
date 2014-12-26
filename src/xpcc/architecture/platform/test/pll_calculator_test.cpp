// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#include "pll_calculator_test.hpp"
#include "../driver/clock/stm32/pll_calculator.hpp"
#include "../driver/clock/lpc/pll_calculator.hpp"

	/* This file tests the various compile time
	 * Pll constants calculators.
	 * Since these use static asserts, things will
	 * break during compile time.
	 */

void
PllCalculatorTest::testStm32F1()
{
	// Go through some common values

}

#define testF2(in, out, usb) \
	(void)xpcc::stm32::Stm32F2F4PllSettings<MHz64, in, out, usb>::PllM

void
PllCalculatorTest::testStm32F2()
{
	// Go through some common values
	// SystemClock: 120MHz
	testF2(MHz8,  MHz120, MHz48);
	testF2(MHz16, MHz120, MHz48);
	testF2(MHz20, MHz120, MHz48);
	testF2(MHz25, MHz120, MHz48);
	// SystemClock: 64MHz
	testF2(MHz8,  MHz64, MHz48);
	testF2(MHz16, MHz64, MHz48);
	testF2(MHz20, MHz64, MHz48);
	testF2(MHz25, MHz64, MHz48);
	// SystemClock: 48MHz
	testF2(MHz8,  MHz48, MHz48);
	testF2(MHz16, MHz48, MHz48);
	testF2(MHz20, MHz48, MHz48);
	testF2(MHz25, MHz48, MHz48);
}

#define testF3HSE(in, out) \
	(void)xpcc::stm32::Stm32F3PllSettings<in, out, false>::PllMul
#define testF3HSI(out) \
	(void)xpcc::stm32::Stm32F3PllSettings<MHz8, out, true>::PllMul

void
PllCalculatorTest::testStm32F3()
{
	// Go through some common values
	// For HSE a 1-16 prediv can be selected
	// SystemClock: 72MHz
	testF3HSE(MHz8,  MHz72);
	testF3HSE(MHz12, MHz72);
	testF3HSE(MHz16, MHz72);
	testF3HSE(MHz18, MHz72);
	testF3HSE(MHz24, MHz72);
	testF3HSE(MHz32, MHz72);
	const bool CanUseUSB72MHz =
				xpcc::stm32::Stm32F3PllSettings<MHz32, MHz72, false>::CanUseUSB;
	TEST_ASSERT_EQUALS(CanUseUSB72MHz, true);
	// SystemClock: 64MHz
	testF3HSE(MHz8,  MHz64);
	testF3HSE(MHz12, MHz64);
	testF3HSE(MHz16, MHz64);
	testF3HSE(MHz24, MHz64);
	testF3HSE(MHz32, MHz64);
	const bool CanUseUSB64MHz =
				xpcc::stm32::Stm32F3PllSettings<MHz32, MHz64, false>::CanUseUSB;
	TEST_ASSERT_EQUALS(CanUseUSB64MHz, false);
	// SystemClock: 48MHz
	testF3HSE(MHz8,  MHz48);
	testF3HSE(MHz12, MHz48);
	testF3HSE(MHz16, MHz48);
	testF3HSE(MHz24, MHz48);
	testF3HSE(MHz32, MHz48);
	const bool CanUseUSB48MHz =
				xpcc::stm32::Stm32F3PllSettings<MHz32, MHz48, false>::CanUseUSB;
	TEST_ASSERT_EQUALS(CanUseUSB48MHz, true);
	// SystemClock: 32MHz
	testF3HSE(MHz8,  MHz32);
	testF3HSE(MHz12, MHz32);
	testF3HSE(MHz16, MHz32);
	testF3HSE(MHz24, MHz32);
	testF3HSE(MHz32, MHz32);
	const bool CanUseUSB32MHz =
				xpcc::stm32::Stm32F3PllSettings<MHz32, MHz32, false>::CanUseUSB;
	TEST_ASSERT_EQUALS(CanUseUSB32MHz, false);
	// Source HSI/2
	// SystemClock: 32MHz
	testF3HSI(MHz64);
	testF3HSI(MHz48);
	testF3HSI(MHz32);
	testF3HSI(MHz16);
}

#define testF4(in, out, usb) \
	(void)xpcc::stm32::Stm32F2F4PllSettings<MHz192, in, out, usb>::PllM

void
PllCalculatorTest::testStm32F4()
{
	// Go through some common values
	// SystemClock: 168MHz
	testF4(MHz8,  MHz168, MHz48);
	testF4(MHz16, MHz168, MHz48);
	testF4(MHz20, MHz168, MHz48);
	testF4(MHz25, MHz168, MHz48);
	// SystemClock: 120MHz
	testF4(MHz8,  MHz120, MHz48);
	testF4(MHz16, MHz120, MHz48);
	testF4(MHz20, MHz120, MHz48);
	testF4(MHz25, MHz120, MHz48);
	// SystemClock: 64MHz
	testF4(MHz8,  MHz64, MHz48);
	testF4(MHz16, MHz64, MHz48);
	testF4(MHz20, MHz64, MHz48);
	testF4(MHz25, MHz64, MHz48);
}

#define testLPC11(in, out) \
	(void)xpcc::lpc::Lpc11PllSettings<in, out>::PllM

void
PllCalculatorTest::testLPC11XX()
{
	// Go through some common values
	// SystemClock: 20MHz
	testLPC11(MHz10, MHz20);
	testLPC11(MHz16, MHz20);
	testLPC11(MHz20, MHz20);
	// SystemClock: 32MHz
	testLPC11(MHz16, MHz32);
	// SystemClock: 36MHz
	testLPC11(MHz12, MHz36);
	// SystemClock: 48MHz
	testLPC11(MHz12, MHz48);
	testLPC11(MHz16, MHz48);
}

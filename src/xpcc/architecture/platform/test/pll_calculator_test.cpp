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
#include "../peripheral/clock/stm32/pll_calculator.hpp"

	/* This file tests the various compile time
	 * Pll constants calculators.
	 * Sinc these use static asserts, things will
	 * break during compile time.
	 */

void
PllCalculatorTest::testStm32F1()
{
	// Go through some common values

}

#define testF2(in, out, usb) \
	xpcc::stm32::Stm32F2F4PllSettings<MHz64, in, out, usb>::PllM

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

void
PllCalculatorTest::testStm32F3()
{
	// Go through some common values

}

#define testF4(in, out, usb) \
	xpcc::stm32::Stm32F2F4PllSettings<MHz192, in, out, usb>::PllM

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
	xpcc::lpc::Lpc11PllSettings<in, out>::PllM

void
PllCalculatorTest::testLPC11XX()
{
	// Go through some common values
	// SystemClock: 20MHz
	testLPC11(MHz10, MHz20);
	testLPC11(MHz12, MHz20);
	testLPC11(MHz16, MHz20);
	testLPC11(MHz20, MHz20);
	// SystemClock: 32MHz
	testLPC11(MHz10, MHz32);
	testLPC11(MHz12, MHz32);
	testLPC11(MHz16, MHz32);
	testLPC11(MHz20, MHz32);
	// SystemClock: 48MHz
	testLPC11(MHz10, MHz48);
	testLPC11(MHz12, MHz48);
	testLPC11(MHz16, MHz48);
	testLPC11(MHz20, MHz48);

}

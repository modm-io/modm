// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <xpcc/hal/peripheral/avr/math/math.hpp>

#include "math_test.hpp"

void
MathTest::testSqrt()
{
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(0), 0);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(1), 1);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(10), 3);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(100), 10);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(1000), 32);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(10000), 100);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(100000), 316);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(123456), 351);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(452613), 673);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(1000000), 1000);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(10000000), 3162);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(4000000000UL), 63245);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_round(4294800000UL), 65535);
	
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(0), 0);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(1), 1);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(10), 3);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(100), 10);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(1000), 31);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(10000), 100);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(100000), 316);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(123456), 351);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(452613), 672);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(1000000), 1000);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(10000000), 3162);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(4000000000UL), 63245);
	TEST_ASSERT_EQUALS(xpcc::avr::sqrt32_floor(4294967295UL), 65535);
}

void
MathTest::testMultiplication()
{
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((uint16_t) 1, (uint16_t) 1), 1);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((uint16_t) 10, (uint16_t) 10), 100);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((uint16_t) 100, (uint16_t) 100), 10000);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((uint16_t) 1000, (uint16_t) 1000), 1000000);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((uint16_t) 10000, (uint16_t) 10000), 100000000);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((uint16_t) 60000, (uint16_t) 60000), 3600000000UL);
	
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) 1, (int16_t) 1), 1);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) 10, (int16_t) 10), 100);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) 100, (int16_t) 100), 10000);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) 1000, (int16_t) 1000), 1000000);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) 10000, (int16_t) 10000), 100000000);
	
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) -1, (int16_t) -1), 1);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) -10, (int16_t) 10), -100);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) 100, (int16_t) -100), -10000);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) 1000, (int16_t) 1000), 1000000);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) 32000, (int16_t) 32000), 1024000000);
	TEST_ASSERT_EQUALS(xpcc::avr::mul32((int16_t) -32000, (int16_t) -32000), 1024000000);
}

void
MathTest::testMultiplyAccumulate()
{
	int32_t offset = -2130;
	
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) 1, (int16_t) 1), 1 - 2130);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) 10, (int16_t) 10), 100 - 2130);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) 100, (int16_t) 100), 10000 - 2130);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) 1000, (int16_t) 1000), 1000000 - 2130);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) 10000, (int16_t) 10000), 100000000 - 2130);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) -10000, (int16_t) -10000), 100000000 - 2130);
	
	offset = 1235678;
	
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) -1, (int16_t) -1), 1 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) -10, (int16_t) 10), -100 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) 100, (int16_t) -100), -10000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) 1000, (int16_t) 1000), 1000000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) -1000, (int16_t) 1000), -1000000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) 32000, (int16_t) 32000), 1024000000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) -32000, (int16_t) -32000), 1024000000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::avr::mac32(offset, (int16_t) -32000, (int16_t) 32000), -1024000000 + 1235678);
}

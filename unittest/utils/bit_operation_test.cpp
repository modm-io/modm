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

#include <xpcc/utils/misc.hpp>

#include "bit_operation_test.hpp"

using namespace xpcc;

void
BitOperationTest::testSwap()
{
	TEST_ASSERT_EQUALS(utils::swap((uint8_t) 0xab), 0xba);
	TEST_ASSERT_EQUALS(utils::swap((uint8_t) 0x18), 0x81);
	TEST_ASSERT_EQUALS(utils::swap((uint8_t) 0x0f), 0xf0);
	TEST_ASSERT_EQUALS(utils::swap((uint8_t) 0x73), 0x37);
	
	TEST_ASSERT_EQUALS(utils::swap((uint16_t) 0xab45), 0x45ab);
	TEST_ASSERT_EQUALS(utils::swap((uint16_t) 0x18ab), 0xab18);
	TEST_ASSERT_EQUALS(utils::swap((uint16_t) 0x0f26), 0x260f);
	TEST_ASSERT_EQUALS(utils::swap((uint16_t) 0x73f7), 0xf773);
}

void
BitOperationTest::testReverse8bit()
{
	uint8_t i, k;
	for (i = 1, k = 0x80; i != 0; i <<= 1, k >>= 1)
	{
		TEST_ASSERT_EQUALS(utils::bitReverse(i), k);
	}
	
	// test some random values
	TEST_ASSERT_EQUALS(utils::bitReverse((uint8_t) 0xa3), 0xc5);
	TEST_ASSERT_EQUALS(utils::bitReverse((uint8_t) 0x0f), 0xf0);
	TEST_ASSERT_EQUALS(utils::bitReverse((uint8_t) 0xc1), 0x83);
}

void
BitOperationTest::testReverse16bit()
{
	uint16_t i, k;
	for (i = 1, k = 0x8000; i != 0; i <<= 1, k >>= 1)
	{
		TEST_ASSERT_EQUALS(utils::bitReverse(i), k);
	}
	
	// test some random values
	TEST_ASSERT_EQUALS(utils::bitReverse((uint16_t) 0xa302), 0x40c5);
	TEST_ASSERT_EQUALS(utils::bitReverse((uint16_t) 0x00f0), 0x0f00);
	TEST_ASSERT_EQUALS(utils::bitReverse((uint16_t) 0xc176), 0x6e83);
}

void
BitOperationTest::testReverse32bit()
{
	uint32_t i, k;
	for (i = 1, k = 0x80000000; i != 0; i <<= 1, k >>= 1)
	{
		TEST_ASSERT_EQUALS(utils::bitReverse(i), k);
	}
	
	// test some random values
	TEST_ASSERT_EQUALS(utils::bitReverse((uint32_t) 0xa3010ff0), 0x0ff080c5);
	TEST_ASSERT_EQUALS(utils::bitReverse((uint32_t) 0xa3010ff0), 0x0ff080c5);
	TEST_ASSERT_EQUALS(utils::bitReverse((uint32_t) 0x0f0c1760), 0x06e830f0);
	TEST_ASSERT_EQUALS(utils::bitReverse((uint32_t) 0xc1040aaa), 0x55502083);
}

// ----------------------------------------------------------------------------

void
BitOperationTest::testCount8bit()
{
	uint8_t i, k;
	for (i = 0, k = 0; i < 9; i++)
	{
		TEST_ASSERT_EQUALS(utils::bitCount(k), i);
		
		k <<= 1;
		k |= 1;
	}
	
	TEST_ASSERT_EQUALS(utils::bitCount((uint8_t) 0x00), 0);
	TEST_ASSERT_EQUALS(utils::bitCount((uint8_t) 0x3c), 4);
	TEST_ASSERT_EQUALS(utils::bitCount((uint8_t) 0x80), 1);
	TEST_ASSERT_EQUALS(utils::bitCount((uint8_t) 0x07), 3);
	TEST_ASSERT_EQUALS(utils::bitCount((uint8_t) 0xff), 8);
}

void
BitOperationTest::testCount16bit()
{
	uint16_t i, k;
	for (i = 0, k = 0; i < 17; i++)
	{
		TEST_ASSERT_EQUALS(utils::bitCount(k), i);
		
		k <<= 1;
		k |= 1;
	}
	
	TEST_ASSERT_EQUALS(utils::bitCount((uint16_t) 0x0000), 0);
	TEST_ASSERT_EQUALS(utils::bitCount((uint16_t) 0x3c00), 4);
	TEST_ASSERT_EQUALS(utils::bitCount((uint16_t) 0x0080), 1);
	TEST_ASSERT_EQUALS(utils::bitCount((uint16_t) 0xf307), 9);
	TEST_ASSERT_EQUALS(utils::bitCount((uint16_t) 0xffff), 16);
}

void
BitOperationTest::testCount32bit()
{
	uint32_t i, k;
	for (i = 0, k = 0; i < 33; i++)
	{
		TEST_ASSERT_EQUALS(utils::bitCount(k), i);
		
		k <<= 1;
		k |= 1;
	}
	
	TEST_ASSERT_EQUALS(utils::bitCount((uint32_t) 0x00000000), 0);
	TEST_ASSERT_EQUALS(utils::bitCount((uint32_t) 0x00003c00), 4);
	TEST_ASSERT_EQUALS(utils::bitCount((uint32_t) 0x0560f307), 13);
	TEST_ASSERT_EQUALS(utils::bitCount((uint32_t) 0xffffffff), 32);
}

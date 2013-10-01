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
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/utils/bit_operation.hpp>

#include "bit_operation_test.hpp"

void
BitOperationTest::testSwap()
{
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint8_t>(0xab)), 0xbaU);
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint8_t>(0x18)), 0x81U);
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint8_t>(0x0f)), 0xf0U);
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint8_t>(0x73)), 0x37U);

	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint16_t>(0xab45)), 0x45abU);
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint16_t>(0x18ab)), 0xab18U);
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint16_t>(0x0f26)), 0x260fU);
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint16_t>(0x73f7)), 0xf773U);

	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint32_t>(0xab45de90)), 0x90de45abU);
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint32_t>(0x18ab28cf)), 0xcf28ab18U);
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint32_t>(0x0f26fe87)), 0x87fe260fU);
	TEST_ASSERT_EQUALS(xpcc::swap(static_cast<uint32_t>(0x73f72234)), 0x3422f773U);
}

void
BitOperationTest::testReverse8bit()
{
	uint8_t i, k;
	for (i = 1, k = 0x80; i != 0; i <<= 1, k >>= 1)
	{
		TEST_ASSERT_EQUALS(xpcc::bitReverse(i), k);
	}

	// test some random values
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint8_t>(0xa3)), 0xc5U);
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint8_t>(0x0f)), 0xf0U);
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint8_t>(0xc1)), 0x83U);
}

void
BitOperationTest::testReverse16bit()
{
	uint16_t i, k;
	for (i = 1, k = 0x8000; i != 0; i <<= 1, k >>= 1)
	{
		TEST_ASSERT_EQUALS(xpcc::bitReverse(i), k);
	}

	// test some random values
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint16_t>(0xa302)), 0x40c5U);
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint16_t>(0x00f0)), 0x0f00U);
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint16_t>(0xc176)), 0x6e83U);
}

void
BitOperationTest::testReverse32bit()
{
	uint32_t i, k;
	for (i = 1, k = 0x80000000; i != 0; i <<= 1, k >>= 1)
	{
		TEST_ASSERT_EQUALS(xpcc::bitReverse(i), k);
	}

	// test some random values
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint32_t>(0xa3010ff0)), 0x0ff080c5U);
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint32_t>(0xa3010ff0)), 0x0ff080c5U);
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint32_t>(0x0f0c1760)), 0x06e830f0U);
	TEST_ASSERT_EQUALS(xpcc::bitReverse(static_cast<uint32_t>(0xc1040aaa)), 0x55502083U);
}

// ----------------------------------------------------------------------------

void
BitOperationTest::testCount8bit()
{
	uint8_t i, k;
	for (i = 0, k = 0; i < 9; i++)
	{
		TEST_ASSERT_EQUALS(xpcc::bitCount(k), i);

		k <<= 1;
		k |= 1;
	}

	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint8_t>(0x00)), 0U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint8_t>(0x3c)), 4U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint8_t>(0x80)), 1U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint8_t>(0x07)), 3U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint8_t>(0xaa)), 4U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint8_t>(0x55)), 4U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint8_t>(0xff)), 8U);
}

void
BitOperationTest::testCount16bit()
{
	uint16_t i, k;
	for (i = 0, k = 0; i < 17; i++)
	{
		TEST_ASSERT_EQUALS(xpcc::bitCount(k), i);

		k <<= 1;
		k |= 1;
	}

	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint16_t>(0x0000)), 0U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint16_t>(0x3c00)), 4U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint16_t>(0x0080)), 1U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint16_t>(0xaaaa)), 8U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint16_t>(0x5555)), 8U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint16_t>(0xf307)), 9U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint16_t>(0xffff)), 16U);
}

void
BitOperationTest::testCount32bit()
{
	uint32_t i, k;
	for (i = 0, k = 0; i < 33; i++)
	{
		TEST_ASSERT_EQUALS(xpcc::bitCount(k), i);

		k <<= 1;
		k |= 1;
	}

	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint32_t>(0x00000000)), 0U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint32_t>(0x00003c00)), 4U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint32_t>(0x0560f307)), 13U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint32_t>(0xaaaaaaaa)), 16U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint32_t>(0x55555555)), 16U);
	TEST_ASSERT_EQUALS(xpcc::bitCount(static_cast<uint32_t>(0xffffffff)), 32U);
}

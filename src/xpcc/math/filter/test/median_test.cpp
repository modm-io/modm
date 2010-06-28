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

#include <xpcc/math/filter/median.hpp>

#include "median_test.hpp"

namespace
{
	struct TestData
	{
		uint8_t inputValue;
		uint8_t median3;
		uint8_t median5;
		uint8_t median7;
		uint8_t median9;
	};
	
	static const TestData testData[] =
	{
		{ 5,	5, 5, 5, 5 },
		{ 100,	5, 5, 5, 5 },
		{ 100,	100, 5, 5, 5 },
		{ 100,	100, 100, 5, 5 },
		{ 100,	100, 100, 100, 5 },
		{ 100,	100, 100, 100, 100 },
		{ 100,	100, 100, 100, 100 },
		{ 200,	100, 100, 100, 100 },
		{ 200,	200, 100, 100, 100 },
		{ 100,	200, 100, 100, 100 },
		{ 100,	100, 100, 100, 100 },
		{ 200,	100, 200, 100, 100 },
		{ 200,	200, 200, 200, 100 },
		{ 200,	200, 200, 200, 200 },
		{ 200,	200, 200, 200, 200 },
		{ 200,	200, 200, 200, 200 },
		{ 10,	200, 200, 200, 200 },
		{ 20,	20, 200, 200, 200 },
		{ 10,	10, 20, 200, 200 },
		{ 20,	20, 20, 20, 200 },
		{ 10,	10, 10, 20, 20 },
		{ 10,	10, 10, 10, 20 },
		{ 10,	10, 10, 10, 10 },
		{ 10,	10, 10, 10, 10 },
	};
}

void
MedianTest::testDefaultConstructor()
{
	xpcc::filter::Median<uint8_t, 3> filter3;
	xpcc::filter::Median<uint8_t, 5> filter5;
	xpcc::filter::Median<uint8_t, 7> filter7;
	xpcc::filter::Median<uint8_t, 9> filter9;
	
	TEST_ASSERT_EQUALS(filter3.getValue(), 0);
	TEST_ASSERT_EQUALS(filter5.getValue(), 0);
	TEST_ASSERT_EQUALS(filter7.getValue(), 0);
	TEST_ASSERT_EQUALS(filter9.getValue(), 0);
}

void
MedianTest::testMedian()
{
	xpcc::filter::Median<uint8_t, 3> filter3(5);
	xpcc::filter::Median<uint8_t, 5> filter5(5);
	xpcc::filter::Median<uint8_t, 7> filter7(5);
	xpcc::filter::Median<uint8_t, 9> filter9(5);
	
	TEST_ASSERT_EQUALS(filter3.getValue(), 5);
	TEST_ASSERT_EQUALS(filter5.getValue(), 5);
	TEST_ASSERT_EQUALS(filter7.getValue(), 5);
	TEST_ASSERT_EQUALS(filter9.getValue(), 5);
	
	for (unsigned int i = 0; i < (sizeof(testData) / sizeof(TestData)); ++i)
	{
		filter3.append(testData[i].inputValue);
		filter5.append(testData[i].inputValue);
		filter7.append(testData[i].inputValue);
		filter9.append(testData[i].inputValue);
		
		filter3.update();
		filter5.update();
		filter7.update();
		filter9.update();
		
		TEST_ASSERT_EQUALS(filter3.getValue(), testData[i].median3);
		TEST_ASSERT_EQUALS(filter5.getValue(), testData[i].median5);
		TEST_ASSERT_EQUALS(filter7.getValue(), testData[i].median7);
		TEST_ASSERT_EQUALS(filter9.getValue(), testData[i].median9);
	}
}

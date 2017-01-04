/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/filter/median.hpp>

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

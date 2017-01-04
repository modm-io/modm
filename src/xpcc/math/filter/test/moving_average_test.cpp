/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Thorsten Lajewski
 * Copyright (c) 2015, Kevin Laeufer
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <cmath>

#include <modm/math/filter/moving_average.hpp>

#include "moving_average_test.hpp"

namespace
{
    struct TestData
	{
		typedef int16_t Type;
		
		Type input;
		Type output;
	};

	struct TestDataFloat
	{
		typedef double Type;

		Type input;
		Type output;
	};

	static const TestData data[] =
	{
		{ 10,	2 },
		{ 10,	5 },
		{ 10,	7 },
		{ 10,	10 },
		{ -10,	5 },
		{ -10,	0 },
		{ 5,	-2 },
		{ 5,	-3 },
		{ 5,	1 },
		{ 5,	5 },
		{ 100,	28 },
		{ 100,	52 },
		{ 200,	101 },
		{ 200,	150 },
		{ 200,	175 },
		{ 200,	200 },
	};

	static const TestDataFloat dataF[] =
	{
		{ 10.0,	2.5 },
		{ 10.0,	5.0 },
		{ 10.0,	7.5 },
		{ 10.0,	10.0 },
		{ -10.0,	5.0 },
		{ -10.0,	0.0 },
		{ 5,	-1.25 },
		{ 5,	-2.5 },
		{ 5,	1.25 },
		{ 5,	5.0 },
		{ 100,	28.75 },
		{ 100,	52.5 },
		{ 200,	101.25 },
		{ 200,	150.0 },
		{ 200,	175.0 },
		{ 200,	200.0 },
	};
}

void
MovingAverageTest::testDefaultConstructor()
{
	xpcc::filter::MovingAverage<int16_t, 4> filter;

	TEST_ASSERT_EQUALS(filter.getValue(), 0);
}

void
MovingAverageTest::testConstructor()
{
	xpcc::filter::MovingAverage<int16_t, 4> filter(20);

	TEST_ASSERT_EQUALS(filter.getValue(), 20);
}

void
MovingAverageTest::testAverage()
{
	xpcc::filter::MovingAverage<TestData::Type, 4> filter;

	for (uint_fast8_t i = 0; i < (sizeof(data) / sizeof(TestData)); ++i)
	{
		filter.update(data[i].input);
		TEST_ASSERT_EQUALS(filter.getValue(), data[i].output);
	}
}

void
MovingAverageTest::testFloatAverage()
{
	xpcc::filter::MovingAverage<TestDataFloat::Type, 4> filter;
	for (uint_fast8_t i = 0; i < (sizeof(data) / sizeof(TestDataFloat)); ++i)
	{
		filter.update(dataF[i].input);
		TEST_ASSERT_EQUALS_DELTA(filter.getValue(), dataF[i].output, double(1e-4));
	}
}

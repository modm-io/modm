/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/filter/debounce.hpp>

#include "debounce_test.hpp"

namespace
{
	struct TestData
	{
		bool input;
		bool output;
	};
	
	static const TestData testData[] =
	{
		{ true, false },
		{ true, true },
		{ true, true },
		{ false, true },
		{ false, true },
		{ false, true },
		{ false, false },
		{ false, false },
		{ false, false },
		{ true, false },
		{ true, false },
		{ true, false },
		{ true, true },
	};
}

void
DebounceTest::testDebounce()
{
	xpcc::filter::Debounce<> filter(4);
	
	TEST_ASSERT_FALSE(filter.getValue());
	
	for (unsigned int i = 0; i < (sizeof(testData) / sizeof(TestData)); ++i)
	{
		filter.update(testData[i].input);
		TEST_ASSERT_EQUALS(testData[i].output, filter.getValue());
	}
}

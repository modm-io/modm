/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/algorithm/range.hpp>
#include "range_test.hpp"

void
RangeTest::testRange()
{
	// Positive step
	{
		int counter{0};
		for (auto ii : modm::range(10))
		{
			TEST_ASSERT_EQUALS(ii, counter);
			counter++;
		}
		TEST_ASSERT_EQUALS(10, counter);
	}
	{
		int counter{0};
		for (auto ii : modm::range(0, 10, 2))
		{
			TEST_ASSERT_EQUALS(ii, counter);
			counter += 2;
		}
		TEST_ASSERT_EQUALS(10, counter);
	}
	{
		int counter{5};
		for (auto ii : modm::range(5, 15))
		{
			TEST_ASSERT_EQUALS(ii, counter);
			counter++;
		}
		TEST_ASSERT_EQUALS(15, counter);
	}
	{
		int counter{5};
		for (auto ii : modm::range(5, 15, 2))
		{
			TEST_ASSERT_EQUALS(ii, counter);
			counter += 2;
		}
		TEST_ASSERT_EQUALS(15, counter);
	}
	// Negative step
	{
		int counter{10};
		for (auto ii : modm::range(10, 0, -1))
		{
			TEST_ASSERT_EQUALS(ii, counter);
			counter--;
		}
		TEST_ASSERT_EQUALS(0, counter);
	}
	{
		int counter{10};
		for (auto ii : modm::range(10, 0, -2))
		{
			TEST_ASSERT_EQUALS(ii, counter);
			counter -= 2;
		}
		TEST_ASSERT_EQUALS(0, counter);
	}
	{
		int counter{15};
		for (auto ii : modm::range(15, 5, -1))
		{
			TEST_ASSERT_EQUALS(ii, counter);
			counter--;
		}
		TEST_ASSERT_EQUALS(5, counter);
	}
	// constexpr calls
	{
		auto get_counter = []() constexpr
		{
			int counter{0};
			for (auto ii : modm::range(10))
				counter += ii;
			return counter;
		};
		static_assert(45 == get_counter());
	}
	{
		auto get_counter = []() constexpr
		{
			int counter{0};
			for (auto ii : modm::range(5, 10, 2))
				counter += ii;
			return counter;
		};
		static_assert(21 == get_counter());
	}
	{
		auto get_counter = []() constexpr
		{
			int counter{0};
			for (auto ii : modm::range(10, 5, -2))
				counter += ii;
			return counter;
		};
		static_assert(24 == get_counter());
	}
}


void
RangeTest::testInvalidRange()
{
	// Invalid ranges should be empty!
	{
		int counter{0};
		for (auto ii : modm::range(0))
			counter += ii;
		TEST_ASSERT_EQUALS(0, counter);
	}
	{
		int counter{0};
		for (auto ii : modm::range(1, 1))
			counter += ii;
		TEST_ASSERT_EQUALS(0, counter);
	}
	{
		int counter{0};
		for (auto ii : modm::range(1, 1, -1))
			counter += ii;
		TEST_ASSERT_EQUALS(0, counter);
	}
	{
		int counter{0};
		for (auto ii : modm::range(-10))
			counter += ii;
		TEST_ASSERT_EQUALS(0, counter);
	}
	{
		int counter{0};
		for (auto ii : modm::range(10, 0))
			counter += ii;
		TEST_ASSERT_EQUALS(0, counter);
	}
	{
		int counter{0};
		for (auto ii : modm::range(0, 10, -1))
			counter += ii;
		TEST_ASSERT_EQUALS(0, counter);
	}
	{
		int counter{0};
		for (auto ii : modm::range(0, 10, 0))
			counter += ii;
		TEST_ASSERT_EQUALS(0, counter);
	}
	{
		int counter{0};
		for (auto ii : modm::range(10, 0, 0))
			counter += ii;
		TEST_ASSERT_EQUALS(0, counter);
	}
	// constexpr
	{
		auto get_counter = []() constexpr
		{
			int counter{0};
			for (auto ii : modm::range(0))
				counter += ii;
			return counter;
		};
		static_assert(0 == get_counter());
	}
	{
		auto get_counter = []() constexpr
		{
			int counter{0};
			for (auto ii : modm::range(10, 0))
				counter += ii;
			return counter;
		};
		static_assert(0 == get_counter());
	}
	{
		auto get_counter = []() constexpr
		{
			int counter{0};
			for (auto ii : modm::range(0, 10, -1))
				counter += ii;
			return counter;
		};
		static_assert(0 == get_counter());
	}
}

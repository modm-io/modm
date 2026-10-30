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

static_assert(std::ranges::random_access_range<decltype(modm::range(10))>);
static_assert(std::ranges::size(modm::range(0, 10, 3)) == 4);
static_assert(std::ranges::size(modm::range(10, 0, -3)) == 4);
// steps that would overflow the type end the range correctly
static_assert(std::ranges::size(modm::range<uint8_t>(0, 255, 100)) == 3);
static_assert(modm::range<uint8_t>(0, 255, 100)[2] == 200);
static_assert(std::ranges::size(modm::range<int8_t>(100, 127, 20)) == 2);
static_assert(std::ranges::size(modm::range<int8_t>(-128, 127, 127)) == 3);
static_assert(modm::range<int8_t>(127, -128, -100)[2] == -73);
static_assert(std::ranges::size(modm::range(0.f, 1.f, 0.25f)) == 4);

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
		auto get_counter = [] constexpr
		{
			int counter{0};
			for (auto ii : modm::range(10))
				counter += ii;
			return counter;
		};
		static_assert(45 == get_counter());
	}
	{
		auto get_counter = [] constexpr
		{
			int counter{0};
			for (auto ii : modm::range(5, 10, 2))
				counter += ii;
			return counter;
		};
		static_assert(21 == get_counter());
	}
	{
		auto get_counter = [] constexpr
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
		auto get_counter = [] constexpr
		{
			int counter{0};
			for (auto ii : modm::range(0))
				counter += ii;
			return counter;
		};
		static_assert(0 == get_counter());
	}
	{
		auto get_counter = [] constexpr
		{
			int counter{0};
			for (auto ii : modm::range(10, 0))
				counter += ii;
			return counter;
		};
		static_assert(0 == get_counter());
	}
	{
		auto get_counter = [] constexpr
		{
			int counter{0};
			for (auto ii : modm::range(0, 10, -1))
				counter += ii;
			return counter;
		};
		static_assert(0 == get_counter());
	}
}

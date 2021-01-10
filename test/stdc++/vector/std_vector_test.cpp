/*
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "std_vector_test.hpp"
#include <vector>
#include <iterator>
#include <algorithm>

void
StdVectorTest::testInsertion()
{
	std::vector<int> vec;

	TEST_ASSERT_EQUALS(vec.size(), 0u);

	std::generate_n(std::back_inserter(vec), 3, [n=1]() mutable { return n *= 2; });
	TEST_ASSERT_TRUE((vec == std::vector{2,4,8}));

	vec.push_back(23);
	vec.push_back(42);
	TEST_ASSERT_TRUE((vec == std::vector{2,4,8,23,42}));

	TEST_ASSERT_EQUALS(vec.size(), 5u);
}

void
StdVectorTest::testIteration()
{
	uint8_t data[] = {1,2,5,6,100};
	std::vector<int> vec{1,2,5,6,100};

	size_t i = 0;
	for(auto value : vec) {
		TEST_ASSERT_EQUALS(value, data[i]);
		++i;
	}

	TEST_ASSERT_EQUALS(i, 5u);
}

void
StdVectorTest::testEmplace()
{
	struct NonCopyable
	{
		int test;
		explicit NonCopyable(int i) : test{i} {}

		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator=(const NonCopyable&) = delete;

		NonCopyable(NonCopyable&&) = default;
		NonCopyable& operator=(NonCopyable&&) = default;
	};

	std::vector<NonCopyable> vec;
	vec.emplace_back(42);

	TEST_ASSERT_EQUALS(vec.front().test, 42);

	std::vector vec2 = std::move(vec);
	TEST_ASSERT_EQUALS(vec.size() , 0u);
	TEST_ASSERT_EQUALS(vec2.size(), 1u);
}

void
StdVectorTest::testRemove()
{
	std::vector<int> vec{2,3,4,5,6,7};
	auto it = vec.begin() + 3;
	it = vec.erase(it);
	vec.erase(it);

	TEST_ASSERT_TRUE((vec == std::vector{2,3,4,7}));
}

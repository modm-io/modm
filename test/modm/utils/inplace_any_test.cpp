/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/utils/inplace_any.hpp>

#include "inplace_any_test.hpp"

struct Test
{
	int value1 = 3;
	int value2 = 2;

	Test() = default;

	Test(int value1, int value2)
		: value1{value1}, value2{value2}
	{}

	Test(const Test& test)
		: value1{test.value1}, value2{test.value2}
	{
		++copyCount;
	}

	Test(Test&& test) noexcept
		: value1{test.value1}, value2{test.value2}
	{
		++moveCount;
	}

	~Test()
	{
		++destructCount;
	}

	auto operator<=>(const Test&) const = default;

	static inline int copyCount = 0;
	static inline int moveCount = 0;
	static inline int destructCount = 0;

	static void reset()
	{
		copyCount = 0;
		moveCount = 0;
		destructCount = 0;
	}
};

using modm::inplace_any;
using modm::any_cast;

void
InplaceAnyTest::testConstruct()
{
	Test::reset();

	// construct empty
	inplace_any emptyAny;
	TEST_ASSERT_TRUE(emptyAny.empty());
	TEST_ASSERT_FALSE(emptyAny.has_value());
	TEST_ASSERT_FALSE(bool(emptyAny));

	// construct from constant value
	inplace_any any1 = 12345678;
	TEST_ASSERT_FALSE(any1.empty());
	TEST_ASSERT_TRUE(any1.has_value());
	TEST_ASSERT_TRUE(bool(any1));
	TEST_ASSERT_TRUE(any_cast<int>(&any1) != nullptr);
	TEST_ASSERT_TRUE(any_cast<char>(&any1) == nullptr);
	TEST_ASSERT_EQUALS(*any_cast<int>(&any1), 12345678);

	// in-place construct
	inplace_any any2{std::in_place_type<Test>, 42, 23};
	Test& test2 = *any_cast<Test>(&any2);
	TEST_ASSERT_TRUE(test2 == Test(42, 23));
	TEST_ASSERT_TRUE((Test::copyCount == 0) && (Test::moveCount == 0));

	// construct and move from r-value
	Test test3{7, 8};
	inplace_any any3 = std::move(test3);
	TEST_ASSERT_TRUE(test3 == *any_cast<Test>(&any3));
	TEST_ASSERT_TRUE((Test::copyCount == 0) && (Test::moveCount == 1));

	// construct and copy
	Test::reset();
	Test test4{1, 6};
	inplace_any any4 = test4;
	TEST_ASSERT_TRUE(test4 == *any_cast<Test>(&any4));
	TEST_ASSERT_TRUE((Test::copyCount == 1) && (Test::moveCount == 0));
}

void
InplaceAnyTest::testCopy()
{
	const inplace_any any1{std::in_place_type<Test>, 42, 23};
	const inplace_any any2{std::in_place_type<Test>, 11, 12};

	// copy-constructor
	Test::reset();
	inplace_any any3 = any1;
	TEST_ASSERT_TRUE((Test::copyCount == 1) && (Test::moveCount == 0));
	TEST_ASSERT_EQUALS(Test::destructCount, 0);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any3) == Test(42, 23));

	// copy-assignment
	Test::reset();
	any3 = any2;
	TEST_ASSERT_TRUE((Test::copyCount == 1) && (Test::moveCount == 0));
	TEST_ASSERT_EQUALS(Test::destructCount, 1);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any3) == Test(11, 12));

	// tests for copy from inplace_any with different size
	const inplace_any<8> any4{std::in_place_type<Test>, 5, 6};
	const inplace_any<8> any5{std::in_place_type<Test>, 7, 8};

	// copy-construct from smaller size
	Test::reset();
	inplace_any any6 = any4;
	TEST_ASSERT_TRUE((Test::copyCount == 1) && (Test::moveCount == 0));
	TEST_ASSERT_EQUALS(Test::destructCount, 0);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any6) == Test(5, 6));

	// copy-assign from smaller size
	Test::reset();
	any6 = any5;
	TEST_ASSERT_TRUE((Test::copyCount == 1) && (Test::moveCount == 0));
	TEST_ASSERT_EQUALS(Test::destructCount, 1);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any6) == Test(7, 8));
}

void
InplaceAnyTest::testMove()
{
	inplace_any any1{std::in_place_type<Test>, 42, 23};
	inplace_any any2{std::in_place_type<Test>, 11, 12};

	// move-constructor
	Test::reset();
	inplace_any any3 = std::move(any1);
	TEST_ASSERT_TRUE((Test::copyCount == 0) && (Test::moveCount == 1));
	TEST_ASSERT_EQUALS(Test::destructCount, 0);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any3) == Test(42, 23));

	// move-assignment
	Test::reset();
	any3 = std::move(any2);
	TEST_ASSERT_TRUE((Test::copyCount == 0) && (Test::moveCount == 1));
	TEST_ASSERT_EQUALS(Test::destructCount, 1);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any3) == Test(11, 12));

	// tests for move from inplace_any with different size
	inplace_any<8> any4{std::in_place_type<Test>, 5, 6};
	inplace_any<8> any5{std::in_place_type<Test>, 7, 8};

	// move-construct from smaller size
	Test::reset();
	inplace_any any6 = std::move(any4);
	TEST_ASSERT_TRUE((Test::copyCount == 0) && (Test::moveCount == 1));
	TEST_ASSERT_EQUALS(Test::destructCount, 0);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any6) == Test(5, 6));

	// move-assign from smaller size
	Test::reset();
	any6 = std::move(any5);
	TEST_ASSERT_TRUE((Test::copyCount == 0) && (Test::moveCount == 1));
	TEST_ASSERT_EQUALS(Test::destructCount, 1);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any6) == Test(7, 8));
}

void
InplaceAnyTest::testAnyCast()
{
	inplace_any any;
	const auto& const_any = any;

	TEST_ASSERT_TRUE(any_cast<int>(&any) == nullptr);
	TEST_ASSERT_TRUE(any_cast<long>(&any) == nullptr);

	// run to trigger assertion:
	// any_cast<int>(any);

	any = -5;
	TEST_ASSERT_TRUE(*any_cast<int>(&any) == -5);
	TEST_ASSERT_TRUE(any_cast<long>(&any) == nullptr);
	TEST_ASSERT_TRUE(*any_cast<int>(&const_any) == -5);
	TEST_ASSERT_TRUE(any_cast<long>(&const_any) == nullptr);
	// run to trigger assertion:
	// any_cast<long>(any);

	// test asserting reference version
	TEST_ASSERT_TRUE(any_cast<int>(any) == -5);
	TEST_ASSERT_TRUE(any_cast<int>(const_any) == -5);
}

void
InplaceAnyTest::testDestruct()
{
	// test destructor
	{
		Test::reset();
		inplace_any any{std::in_place_type<Test>, 42, 23};
	}
	TEST_ASSERT_EQUALS(Test::destructCount, 1);

	// test reset()
	{
		Test::reset();
		inplace_any any{std::in_place_type<Test>, 42, 23};
		any.reset();
		TEST_ASSERT_EQUALS(Test::destructCount, 1);
		TEST_ASSERT_TRUE(any.empty());
		TEST_ASSERT_FALSE(any.has_value());
		TEST_ASSERT_FALSE(bool(any));
	}
	TEST_ASSERT_EQUALS(Test::destructCount, 1);
}

void
InplaceAnyTest::testValueAssignment()
{
	inplace_any any = Test{-100, 23};

	// test value copy-assign
	Test::reset();
	any = 10;
	TEST_ASSERT_TRUE(any_cast<Test>(&any) == nullptr);
	TEST_ASSERT_TRUE(*any_cast<int>(&any) == 10);
	TEST_ASSERT_EQUALS(Test::destructCount, 1);

	// test value move-assign
	auto test = Test{1234, 56};
	Test::reset();
	any = std::move(test);
	TEST_ASSERT_TRUE((Test::copyCount == 0) && (Test::moveCount == 1));
	TEST_ASSERT_EQUALS(Test::destructCount, 0);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any) == test);

	// test emplace() on full any
	Test::reset();
	any.emplace<Test>(-2,-10);
	TEST_ASSERT_TRUE((Test::copyCount == 0) && (Test::moveCount == 0));
	TEST_ASSERT_EQUALS(Test::destructCount, 1);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any) == Test(-2, -10));

	// test emplace() on empty any
	any.reset();
	Test::reset();
	any.emplace<Test>(10, 2);
	TEST_ASSERT_TRUE((Test::copyCount == 0) && (Test::moveCount == 0));
	TEST_ASSERT_EQUALS(Test::destructCount, 0);
	TEST_ASSERT_TRUE(*any_cast<Test>(&any) == Test(10, 2));
}

void
InplaceAnyTest::testSwap()
{
	inplace_any any1 = 1UL;
	inplace_any any2 = short(2);
	inplace_any any3;
	inplace_any any4;

	swap(any1, any2);
	TEST_ASSERT_EQUALS(any_cast<short>(any1), 2);
	TEST_ASSERT_EQUALS(any_cast<unsigned long>(any2), 1UL);

	any1.swap(any2);
	TEST_ASSERT_EQUALS(any_cast<unsigned long>(any1), 1UL);
	TEST_ASSERT_EQUALS(any_cast<short>(any2), 2);

	swap(any1, any1);
	any1.swap(any1);
	TEST_ASSERT_EQUALS(any_cast<unsigned long>(any1), 1UL);

	swap(any3, any3);
	any3.swap(any3);
	TEST_ASSERT_TRUE(any3.empty());

	swap(any1, any3);
	TEST_ASSERT_EQUALS(any_cast<unsigned long>(any3), 1UL);

	swap(any4, any3);
	TEST_ASSERT_EQUALS(any_cast<unsigned long>(any4), 1UL);
}

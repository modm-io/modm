/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2014, 2017, Sascha Schade
 * Copyright (c) 2017, Michael Thies
 * Copyright (c) 2017, Tomasz Chyrowicz
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/container/deque.hpp>
#include <algorithm>
#include <queue>
#include <stack>

#include "bounded_deque_test.hpp"

void
BoundedDequeTest::testForward()
{
	modm::BoundedDeque<int16_t, 3> deque;

	TEST_ASSERT_TRUE(deque.empty());
	TEST_ASSERT_EQUALS(deque.max_size(), 3U);

	TEST_ASSERT_EQUALS(deque.size(), 0U);

	TEST_ASSERT_TRUE(deque.append(1));
	TEST_ASSERT_EQUALS(deque.size(), 1U);
	TEST_ASSERT_TRUE(deque.append(2));
	TEST_ASSERT_EQUALS(deque.size(), 2U);
	TEST_ASSERT_TRUE(deque.append(3));
	TEST_ASSERT_EQUALS(deque.size(), 3U);

	TEST_ASSERT_FALSE(deque.append(4));
	TEST_ASSERT_EQUALS(deque.size(), 3U);
	TEST_ASSERT_TRUE(deque.full());

	TEST_ASSERT_EQUALS(deque.front(), 1);
	deque.pop_front();
	TEST_ASSERT_EQUALS(deque.size(), 2U);

	TEST_ASSERT_EQUALS(deque.front(), 2);
	deque.pop_front();
	TEST_ASSERT_EQUALS(deque.size(), 1U);

	TEST_ASSERT_TRUE(deque.append(4));
	TEST_ASSERT_EQUALS(deque.size(), 2U);
	TEST_ASSERT_TRUE(deque.append(5));
	TEST_ASSERT_EQUALS(deque.size(), 3U);
	TEST_ASSERT_TRUE(deque.full());

	TEST_ASSERT_EQUALS(deque.front(), 3);
	deque.pop_front();
	TEST_ASSERT_EQUALS(deque.size(), 2U);

	TEST_ASSERT_EQUALS(deque.front(), 4);
	deque.pop_front();
	TEST_ASSERT_EQUALS(deque.size(), 1U);

	TEST_ASSERT_EQUALS(deque.front(), 5);
	deque.pop_front();

	TEST_ASSERT_EQUALS(deque.size(), 0U);
	TEST_ASSERT_TRUE(deque.empty());
}

void
BoundedDequeTest::testBackward()
{
	modm::BoundedDeque<int16_t, 3> deque;

	TEST_ASSERT_TRUE(deque.prepend(1));
	TEST_ASSERT_TRUE(deque.prepend(2));
	TEST_ASSERT_TRUE(deque.prepend(3));

	TEST_ASSERT_FALSE(deque.prepend(4));
	TEST_ASSERT_TRUE(deque.full());

	TEST_ASSERT_EQUALS(deque.back(), 1);
	deque.pop_back();

	TEST_ASSERT_EQUALS(deque.back(), 2);
	deque.pop_back();

	TEST_ASSERT_TRUE(deque.prepend(4));
	TEST_ASSERT_TRUE(deque.prepend(5));
	TEST_ASSERT_TRUE(deque.full());

	TEST_ASSERT_EQUALS(deque.back(), 3);
	deque.pop_back();

	TEST_ASSERT_EQUALS(deque.back(), 4);
	deque.pop_back();

	TEST_ASSERT_EQUALS(deque.back(), 5);
	deque.pop_back();

	TEST_ASSERT_TRUE(deque.empty());
}

void
BoundedDequeTest::testBoth()
{
	modm::BoundedDeque<int16_t, 3> deque;

	TEST_ASSERT_EQUALS(deque.size(), 0U);

	TEST_ASSERT_TRUE(deque.prepend(12));
	TEST_ASSERT_EQUALS(deque.size(), 1U);

	TEST_ASSERT_TRUE(deque.prepend(11));
	TEST_ASSERT_EQUALS(deque.size(), 2U);

	TEST_ASSERT_TRUE(deque.prepend(10));
	TEST_ASSERT_EQUALS(deque.size(), 3U);

	TEST_ASSERT_EQUALS(deque.front(), 10);
	deque.pop_front();

	TEST_ASSERT_EQUALS(deque.front(), 11);
	deque.pop_front();

	TEST_ASSERT_TRUE(deque.append(13));
	TEST_ASSERT_TRUE(deque.append(14));
	TEST_ASSERT_TRUE(deque.full());

	TEST_ASSERT_EQUALS(deque.back(), 14);
	deque.pop_back();

	TEST_ASSERT_EQUALS(deque.front(), 12);
	deque.pop_front();

	TEST_ASSERT_EQUALS(deque.back(), 13);
	deque.pop_back();

	TEST_ASSERT_TRUE(deque.empty());
}

void
BoundedDequeTest::testClear()
{
	modm::BoundedDeque<int16_t, 3> deque;

	deque.prepend(12);
	deque.prepend(11);
	deque.prepend(10);

	TEST_ASSERT_EQUALS(deque.size(), 3U);

	deque.clear();

	TEST_ASSERT_EQUALS(deque.size(), 0U);
	TEST_ASSERT_TRUE(deque.empty());

	deque.prepend(12);
	deque.prepend(11);
	deque.prepend(10);
	TEST_ASSERT_EQUALS(deque.size(), 3U);

	TEST_ASSERT_EQUALS(deque.front(), 10);
	deque.pop_front();

	TEST_ASSERT_EQUALS(deque.front(), 11);
	deque.pop_front();

	deque.append(13);
	deque.append(14);
	TEST_ASSERT_TRUE(deque.full());

	TEST_ASSERT_EQUALS(deque.back(), 14);
	deque.pop_back();

	TEST_ASSERT_EQUALS(deque.front(), 12);
	deque.pop_front();

	TEST_ASSERT_EQUALS(deque.back(), 13);
	deque.pop_back();

	TEST_ASSERT_TRUE(deque.empty());
}

void
BoundedDequeTest::testConstIterator()
{
	modm::BoundedDeque<int16_t, 5> deque;

	deque.append(3);
	deque.append(4);
	deque.prepend(2);
	deque.prepend(1);

	modm::BoundedDeque<int16_t, 5>::const_iterator it;
	it = deque.begin();

	TEST_ASSERT_TRUE(it != deque.end());
	TEST_ASSERT_EQUALS(*it, 1);
	++it;

	TEST_ASSERT_TRUE(it != deque.end());
	TEST_ASSERT_EQUALS(*it, 2);
	++it;

	TEST_ASSERT_TRUE(it != deque.end());
	TEST_ASSERT_EQUALS(*it, 3);
	++it;

	TEST_ASSERT_TRUE(it != deque.end());
	TEST_ASSERT_EQUALS(*it, 4);
	++it;

	TEST_ASSERT_FALSE(it != deque.end());
}

void
BoundedDequeTest::testFull()
{
	modm::BoundedDeque<int16_t, 3> deque;

	TEST_ASSERT_TRUE(deque.append(3));
	TEST_ASSERT_TRUE(deque.append(4));
	TEST_ASSERT_TRUE(deque.append(5));
	TEST_ASSERT_FALSE(deque.append(6));
}

void
BoundedDequeTest::testOneElementQueue()
{
	modm::BoundedDeque<int16_t, 1> deque;

	TEST_ASSERT_EQUALS(deque.size(), 0U);
	TEST_ASSERT_EQUALS(deque.max_size(), 1U);

	TEST_ASSERT_TRUE(deque.empty());
	TEST_ASSERT_FALSE(deque.full());

	TEST_ASSERT_TRUE(deque.append(123));
	TEST_ASSERT_EQUALS(deque.size(), 1U);

	TEST_ASSERT_FALSE(deque.empty());
	TEST_ASSERT_TRUE(deque.full());

	TEST_ASSERT_FALSE(deque.append(2));

	TEST_ASSERT_EQUALS(deque.front(), 123);
	TEST_ASSERT_EQUALS(deque.back(), deque.front());

	deque.pop_back();
	TEST_ASSERT_TRUE(deque.empty());
}

void
BoundedDequeTest::testOverwrite()
{
	modm::BoundedDeque<int16_t, 3> deque;

	TEST_ASSERT_TRUE(deque.empty());

	deque.appendOverwrite(1);
	TEST_ASSERT_EQUALS(deque.size(), 1U);
	TEST_ASSERT_TRUE(deque.append(2));
	TEST_ASSERT_EQUALS(deque.size(), 2U);
	deque.appendOverwrite(3);
	TEST_ASSERT_EQUALS(deque.size(), 3U);

	TEST_ASSERT_FALSE(deque.append(4));
	TEST_ASSERT_EQUALS(deque.size(), 3U);
	TEST_ASSERT_EQUALS(deque.front(), 1);
	TEST_ASSERT_EQUALS(deque.back(), 3);
	TEST_ASSERT_TRUE(deque.full());

	deque.appendOverwrite(4);
	TEST_ASSERT_EQUALS(deque.size(), 3U);
	TEST_ASSERT_EQUALS(deque.front(), 2);
	TEST_ASSERT_EQUALS(deque.back(), 4);
	TEST_ASSERT_TRUE(deque.full());

	deque.appendOverwrite(5);
	deque.appendOverwrite(6);
	TEST_ASSERT_EQUALS(deque.size(), 3U);
	TEST_ASSERT_EQUALS(deque.front(), 4);
	TEST_ASSERT_EQUALS(deque.back(), 6);
	TEST_ASSERT_TRUE(deque.full());

	deque.pop_back();
	deque.pop_front();
	TEST_ASSERT_EQUALS(deque.front(), 5);
	TEST_ASSERT_EQUALS(deque.back(), 5);
	deque.pop_front();


	TEST_ASSERT_TRUE(deque.empty());

	deque.prependOverwrite(1);
	TEST_ASSERT_EQUALS(deque.size(), 1U);
	TEST_ASSERT_TRUE(deque.prepend(2));
	TEST_ASSERT_EQUALS(deque.size(), 2U);
	deque.prependOverwrite(3);
	TEST_ASSERT_EQUALS(deque.size(), 3U);

	TEST_ASSERT_FALSE(deque.prepend(4));
	TEST_ASSERT_EQUALS(deque.size(), 3U);
	TEST_ASSERT_EQUALS(deque.back(), 1);
	TEST_ASSERT_EQUALS(deque.front(), 3);
	TEST_ASSERT_TRUE(deque.full());

	deque.prependOverwrite(4);
	TEST_ASSERT_EQUALS(deque.size(), 3U);
	TEST_ASSERT_EQUALS(deque.back(), 2);
	TEST_ASSERT_EQUALS(deque.front(), 4);
	TEST_ASSERT_TRUE(deque.full());

	deque.prependOverwrite(5);
	deque.prependOverwrite(6);
	TEST_ASSERT_EQUALS(deque.size(), 3U);
	TEST_ASSERT_EQUALS(deque.back(), 4);
	TEST_ASSERT_EQUALS(deque.front(), 6);
	TEST_ASSERT_TRUE(deque.full());
}

void
BoundedDequeTest::testElementAccess()
{
	modm::BoundedDeque<int16_t, 4> deque;

	// Fill deque, but assure a wrap around
	deque.append(0);
	deque.append(0);
	deque.append(1);
	deque.append(2);
	TEST_ASSERT_TRUE(deque.full());
	deque.pop_front();
	deque.pop_front();
	deque.append(3);
	deque.append(4);
	TEST_ASSERT_TRUE(deque.full());

	// Test const getter
	TEST_ASSERT_EQUALS(deque[0], 1);
	TEST_ASSERT_EQUALS(deque[1], 2);
	TEST_ASSERT_EQUALS(deque[2], 3);
	TEST_ASSERT_EQUALS(deque[3], 4);
	TEST_ASSERT_EQUALS(deque[0], 1);
	TEST_ASSERT_EQUALS(deque[1], 2);
	TEST_ASSERT_EQUALS(deque[2], 3);
	TEST_ASSERT_EQUALS(deque[3], 4);
	TEST_ASSERT_EQUALS(deque.rbegin()[0], 4);
	TEST_ASSERT_EQUALS(deque.rbegin()[1], 3);
	TEST_ASSERT_EQUALS(deque.rbegin()[2], 2);
	TEST_ASSERT_EQUALS(deque.rbegin()[3], 1);

	// Test const getter
	const auto& constDeque = deque;
	TEST_ASSERT_EQUALS(constDeque[1], 2);
	TEST_ASSERT_EQUALS(constDeque[3], 4);
	TEST_ASSERT_EQUALS(constDeque[0], 1);
	TEST_ASSERT_EQUALS(constDeque.rbegin()[2], 2);

	// Test non-full deque
	deque.pop_front();
	TEST_ASSERT_EQUALS(deque[0], 2);
	TEST_ASSERT_EQUALS(deque[1], 3);
	TEST_ASSERT_EQUALS(deque[2], 4);
	TEST_ASSERT_EQUALS(deque.rbegin()[0], 4);
	TEST_ASSERT_EQUALS(deque.rbegin()[1], 3);
	TEST_ASSERT_EQUALS(deque.rbegin()[2], 2);

}

void
BoundedDequeTest::testIterator()
{
	modm::BoundedDeque<int16_t, 5> deque;
	deque.push_back(3);
	deque.push_back(1);
	deque.push_front(2);
	deque.push_front(4);

	std::sort(deque.begin(), deque.end());
	TEST_ASSERT_EQUALS(deque[0], 1);
	TEST_ASSERT_EQUALS(deque[3], 4);

	for (auto& value : deque) value *= 2;
	TEST_ASSERT_EQUALS(deque.end() - deque.begin(), 4);
	TEST_ASSERT_EQUALS(*(deque.cbegin() + 2), 6);
	TEST_ASSERT_EQUALS(*deque.rbegin(), 8);
	TEST_ASSERT_TRUE(std::find(deque.begin(), deque.end(), 4) == deque.begin() + 1);
}

void
BoundedDequeTest::testAdapter()
{
	std::queue<int16_t, modm::BoundedDeque<int16_t, 3>> queue;
	queue.push(1);
	queue.push(2);
	queue.push(3);
	TEST_ASSERT_EQUALS(queue.size(), 3U);
	TEST_ASSERT_EQUALS(queue.front(), 1);
	queue.pop();
	queue.push(4);
	TEST_ASSERT_EQUALS(queue.front(), 2);
	TEST_ASSERT_EQUALS(queue.back(), 4);

	std::stack<int16_t, modm::BoundedDeque<int16_t, 3>> stack;
	stack.push(1);
	stack.push(2);
	TEST_ASSERT_EQUALS(stack.top(), 2);
	stack.pop();
	TEST_ASSERT_EQUALS(stack.top(), 1);
	stack.pop();
	TEST_ASSERT_TRUE(stack.empty());
}

namespace
{
struct Counted
{
	static inline int16_t instances{0};
	int16_t value;
	Counted(int16_t value) : value(value) { instances++; }
	Counted(const Counted& other) : value(other.value) { instances++; }
	~Counted() { instances--; }
};
}

void
BoundedDequeTest::testNonTrivial()
{
	{
		modm::BoundedDeque<Counted, 3> deque;
		TEST_ASSERT_EQUALS(Counted::instances, 0);

		deque.emplace_back(1);
		deque.emplace_front(0);
		deque.push_back(Counted{2});
		TEST_ASSERT_EQUALS(Counted::instances, 3);
		TEST_ASSERT_EQUALS(deque.front().value, 0);
		TEST_ASSERT_EQUALS(deque.back().value, 2);

		deque.pop_front();
		TEST_ASSERT_EQUALS(Counted::instances, 2);

		auto copy = deque;
		TEST_ASSERT_EQUALS(Counted::instances, 4);
		TEST_ASSERT_EQUALS(copy.front().value, 1);

		copy.clear();
		TEST_ASSERT_EQUALS(Counted::instances, 2);
	}
	TEST_ASSERT_EQUALS(Counted::instances, 0);
}

static_assert([]
{
	modm::BoundedDeque<int16_t, 3> deque;
	deque.push_back(2);
	deque.push_front(1);
	deque.appendOverwrite(3);
	deque.appendOverwrite(4);
	return deque.front() == 2 and deque.back() == 4;
}());


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

#include "bounded_deque_test.hpp"

void
BoundedDequeTest::testForward()
{
	modm::BoundedDeque<int16_t, 3> deque;
	
	TEST_ASSERT_TRUE(deque.isEmpty());
	TEST_ASSERT_EQUALS(deque.getMaxSize(), 3U);
	
	TEST_ASSERT_EQUALS(deque.getSize(), 0U);
	
	TEST_ASSERT_TRUE(deque.append(1));
	TEST_ASSERT_EQUALS(deque.getSize(), 1U);
	TEST_ASSERT_TRUE(deque.append(2));
	TEST_ASSERT_EQUALS(deque.getSize(), 2U);
	TEST_ASSERT_TRUE(deque.append(3));
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	
	TEST_ASSERT_FALSE(deque.append(4));
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.getFront(), 1);
	deque.removeFront();
	TEST_ASSERT_EQUALS(deque.getSize(), 2U);
	
	TEST_ASSERT_EQUALS(deque.getFront(), 2);
	deque.removeFront();
	TEST_ASSERT_EQUALS(deque.getSize(), 1U);
	
	TEST_ASSERT_TRUE(deque.append(4));
	TEST_ASSERT_EQUALS(deque.getSize(), 2U);
	TEST_ASSERT_TRUE(deque.append(5));
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.getFront(), 3);
	deque.removeFront();
	TEST_ASSERT_EQUALS(deque.getSize(), 2U);
	
	TEST_ASSERT_EQUALS(deque.getFront(), 4);
	deque.removeFront();
	TEST_ASSERT_EQUALS(deque.getSize(), 1U);
	
	TEST_ASSERT_EQUALS(deque.getFront(), 5);
	deque.removeFront();
	
	TEST_ASSERT_EQUALS(deque.getSize(), 0U);
	TEST_ASSERT_TRUE(deque.isEmpty());
}

void
BoundedDequeTest::testBackward()
{
	modm::BoundedDeque<int16_t, 3> deque;
	
	TEST_ASSERT_TRUE(deque.prepend(1));
	TEST_ASSERT_TRUE(deque.prepend(2));
	TEST_ASSERT_TRUE(deque.prepend(3));
	
	TEST_ASSERT_FALSE(deque.prepend(4));
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.getBack(), 1);
	deque.removeBack();
	
	TEST_ASSERT_EQUALS(deque.getBack(), 2);
	deque.removeBack();
	
	TEST_ASSERT_TRUE(deque.prepend(4));
	TEST_ASSERT_TRUE(deque.prepend(5));
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.getBack(), 3);
	deque.removeBack();
	
	TEST_ASSERT_EQUALS(deque.getBack(), 4);
	deque.removeBack();
	
	TEST_ASSERT_EQUALS(deque.getBack(), 5);
	deque.removeBack();
	
	TEST_ASSERT_TRUE(deque.isEmpty());
}

void
BoundedDequeTest::testBoth()
{
	modm::BoundedDeque<int16_t, 3> deque;
	
	TEST_ASSERT_EQUALS(deque.getSize(), 0U);
	
	TEST_ASSERT_TRUE(deque.prepend(12));
	TEST_ASSERT_EQUALS(deque.getSize(), 1U);
	
	TEST_ASSERT_TRUE(deque.prepend(11));
	TEST_ASSERT_EQUALS(deque.getSize(), 2U);
	
	TEST_ASSERT_TRUE(deque.prepend(10));
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	
	TEST_ASSERT_EQUALS(deque.getFront(), 10);
	deque.removeFront();
	
	TEST_ASSERT_EQUALS(deque.getFront(), 11);
	deque.removeFront();
	
	TEST_ASSERT_TRUE(deque.append(13));
	TEST_ASSERT_TRUE(deque.append(14));
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.getBack(), 14);
	deque.removeBack();
	
	TEST_ASSERT_EQUALS(deque.getFront(), 12);
	deque.removeFront();
	
	TEST_ASSERT_EQUALS(deque.getBack(), 13);
	deque.removeBack();
	
	TEST_ASSERT_TRUE(deque.isEmpty());
}

void
BoundedDequeTest::testClear()
{
	modm::BoundedDeque<int16_t, 3> deque;
	
	deque.prepend(12);
	deque.prepend(11);
	deque.prepend(10);
	
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	
	deque.clear();
	
	TEST_ASSERT_EQUALS(deque.getSize(), 0U);
	TEST_ASSERT_TRUE(deque.isEmpty());
	
	deque.prepend(12);
	deque.prepend(11);
	deque.prepend(10);
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	
	TEST_ASSERT_EQUALS(deque.getFront(), 10);
	deque.removeFront();
	
	TEST_ASSERT_EQUALS(deque.getFront(), 11);
	deque.removeFront();
	
	deque.append(13);
	deque.append(14);
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.getBack(), 14);
	deque.removeBack();
	
	TEST_ASSERT_EQUALS(deque.getFront(), 12);
	deque.removeFront();
	
	TEST_ASSERT_EQUALS(deque.getBack(), 13);
	deque.removeBack();
	
	TEST_ASSERT_TRUE(deque.isEmpty());
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

	TEST_ASSERT_EQUALS(deque.getSize(), 0U);
	TEST_ASSERT_EQUALS(deque.getMaxSize(), 1U);

	TEST_ASSERT_TRUE(deque.isEmpty());
	TEST_ASSERT_FALSE(deque.isFull());

	TEST_ASSERT_TRUE(deque.append(123));
	TEST_ASSERT_EQUALS(deque.getSize(), 1U);

	TEST_ASSERT_FALSE(deque.isEmpty());
	TEST_ASSERT_TRUE(deque.isFull());

	TEST_ASSERT_FALSE(deque.append(2));

	TEST_ASSERT_EQUALS(deque.getFront(), 123);
	TEST_ASSERT_EQUALS(deque.getBack(), deque.getFront());

	deque.removeBack();
	TEST_ASSERT_TRUE(deque.isEmpty());
}

void
BoundedDequeTest::testOverwrite()
{
	modm::BoundedDeque<int16_t, 3> deque;
	
	TEST_ASSERT_TRUE(deque.isEmpty());
	
	deque.appendOverwrite(1);
	TEST_ASSERT_EQUALS(deque.getSize(), 1U);
	TEST_ASSERT_TRUE(deque.append(2));
	TEST_ASSERT_EQUALS(deque.getSize(), 2U);
	deque.appendOverwrite(3);
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	
	TEST_ASSERT_FALSE(deque.append(4));
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	TEST_ASSERT_EQUALS(deque.getFront(), 1);
	TEST_ASSERT_EQUALS(deque.getBack(), 3);
	TEST_ASSERT_TRUE(deque.isFull());
	
	deque.appendOverwrite(4);
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	TEST_ASSERT_EQUALS(deque.getFront(), 2);
	TEST_ASSERT_EQUALS(deque.getBack(), 4);
	TEST_ASSERT_TRUE(deque.isFull());
	
	deque.appendOverwrite(5);
	deque.appendOverwrite(6);
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	TEST_ASSERT_EQUALS(deque.getFront(), 4);
	TEST_ASSERT_EQUALS(deque.getBack(), 6);
	TEST_ASSERT_TRUE(deque.isFull());
	
	deque.removeBack();
	deque.removeFront();
	TEST_ASSERT_EQUALS(deque.getFront(), 5);
	TEST_ASSERT_EQUALS(deque.getBack(), 5);
	deque.removeFront();
	
	
	TEST_ASSERT_TRUE(deque.isEmpty());
	
	deque.prependOverwrite(1);
	TEST_ASSERT_EQUALS(deque.getSize(), 1U);
	TEST_ASSERT_TRUE(deque.prepend(2));
	TEST_ASSERT_EQUALS(deque.getSize(), 2U);
	deque.prependOverwrite(3);
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	
	TEST_ASSERT_FALSE(deque.prepend(4));
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	TEST_ASSERT_EQUALS(deque.getBack(), 1);
	TEST_ASSERT_EQUALS(deque.getFront(), 3);
	TEST_ASSERT_TRUE(deque.isFull());
	
	deque.prependOverwrite(4);
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	TEST_ASSERT_EQUALS(deque.getBack(), 2);
	TEST_ASSERT_EQUALS(deque.getFront(), 4);
	TEST_ASSERT_TRUE(deque.isFull());
	
	deque.prependOverwrite(5);
	deque.prependOverwrite(6);
	TEST_ASSERT_EQUALS(deque.getSize(), 3U);
	TEST_ASSERT_EQUALS(deque.getBack(), 4);
	TEST_ASSERT_EQUALS(deque.getFront(), 6);
	TEST_ASSERT_TRUE(deque.isFull());
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
	TEST_ASSERT_TRUE(deque.isFull());
	deque.removeFront();
	deque.removeFront();
	deque.append(3);
	deque.append(4);
	TEST_ASSERT_TRUE(deque.isFull());
	
	// Test const getter
	TEST_ASSERT_EQUALS(deque.get(0), 1);
	TEST_ASSERT_EQUALS(deque.get(1), 2);
	TEST_ASSERT_EQUALS(deque.get(2), 3);
	TEST_ASSERT_EQUALS(deque.get(3), 4);
	TEST_ASSERT_EQUALS(deque[0], 1);
	TEST_ASSERT_EQUALS(deque[1], 2);
	TEST_ASSERT_EQUALS(deque[2], 3);
	TEST_ASSERT_EQUALS(deque[3], 4);
	TEST_ASSERT_EQUALS(deque.rget(0), 4);
	TEST_ASSERT_EQUALS(deque.rget(1), 3);
	TEST_ASSERT_EQUALS(deque.rget(2), 2);
	TEST_ASSERT_EQUALS(deque.rget(3), 1);
	
	// Test const getter
	const auto& constDeque = deque;
	TEST_ASSERT_EQUALS(constDeque[1], 2);
	TEST_ASSERT_EQUALS(constDeque[3], 4);
	TEST_ASSERT_EQUALS(constDeque.get(0), 1);
	TEST_ASSERT_EQUALS(constDeque.rget(2), 2);
	
	// Test non-full deque
	deque.removeFront();
	TEST_ASSERT_EQUALS(deque.get(0), 2);
	TEST_ASSERT_EQUALS(deque.get(1), 3);
	TEST_ASSERT_EQUALS(deque.get(2), 4);
	TEST_ASSERT_EQUALS(deque.rget(0), 4);
	TEST_ASSERT_EQUALS(deque.rget(1), 3);
	TEST_ASSERT_EQUALS(deque.rget(2), 2);
	
}

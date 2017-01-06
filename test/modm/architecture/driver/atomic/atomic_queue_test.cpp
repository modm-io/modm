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

#include <modm/architecture/driver/atomic/queue.hpp>

#include "atomic_queue_test.hpp"

void
AtomicQueueTest::testQueue()
{
	modm::atomic::Queue<int16_t, 3> queue;
	
	TEST_ASSERT_TRUE(queue.isEmpty());
	TEST_ASSERT_EQUALS(queue.getMaxSize(), 3);
	
	TEST_ASSERT_TRUE(queue.push(1));
	TEST_ASSERT_TRUE(queue.push(2));
	TEST_ASSERT_TRUE(queue.push(3));
	
	TEST_ASSERT_FALSE(queue.push(4));
	TEST_ASSERT_TRUE(queue.isFull());
	
	TEST_ASSERT_EQUALS(queue.get(), 1);
	queue.pop();
	
	TEST_ASSERT_EQUALS(queue.get(), 2);
	queue.pop();
	
	TEST_ASSERT_TRUE(queue.push(4));
	TEST_ASSERT_TRUE(queue.push(5));
	TEST_ASSERT_TRUE(queue.isFull());
	
	TEST_ASSERT_EQUALS(queue.get(), 3);
	queue.pop();
	
	TEST_ASSERT_EQUALS(queue.get(), 4);
	queue.pop();
	
	TEST_ASSERT_EQUALS(queue.get(), 5);
	queue.pop();
	
	TEST_ASSERT_TRUE(queue.isEmpty());
}

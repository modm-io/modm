/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
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

#include <modm/container/stack.hpp>

#include "bounded_stack_test.hpp"

void
BoundedStackTest::testStack()
{
	modm::BoundedStack<int16_t, 3> stack;
	
	TEST_ASSERT_TRUE(stack.isEmpty());
	TEST_ASSERT_EQUALS(stack.getMaxSize(), 3U);
	
	TEST_ASSERT_TRUE(stack.push(1));
	TEST_ASSERT_TRUE(stack.push(2));
	TEST_ASSERT_TRUE(stack.push(3));
	
	TEST_ASSERT_FALSE(stack.push(4));
	TEST_ASSERT_TRUE(stack.isFull());
	
	TEST_ASSERT_EQUALS(stack.get(), 3);
	stack.pop();
	
	TEST_ASSERT_EQUALS(stack.get(), 2);
	stack.pop();
	
	TEST_ASSERT_TRUE(stack.push(4));
	TEST_ASSERT_TRUE(stack.push(5));
	TEST_ASSERT_TRUE(stack.isFull());
	
	TEST_ASSERT_EQUALS(stack.get(), 5);
	stack.pop();
	
	TEST_ASSERT_EQUALS(stack.get(), 4);
	stack.pop();
	
	TEST_ASSERT_EQUALS(stack.get(), 1);
	stack.pop();
	
	TEST_ASSERT_TRUE(stack.isEmpty());
}

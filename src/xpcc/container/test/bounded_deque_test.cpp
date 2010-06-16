// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <xpcc/container/deque.hpp>

#include "bounded_deque_test.hpp"

void
BoundedDequeTest::testForward()
{
	xpcc::BoundedDeque<int16_t, 3> deque;
	
	TEST_ASSERT_TRUE(deque.isEmpty());
	TEST_ASSERT_EQUALS(deque.getMaxSize(), 3);
	
	TEST_ASSERT_EQUALS(deque.getSize(), 0);
	
	TEST_ASSERT_TRUE(deque.pushBack(1));
	TEST_ASSERT_EQUALS(deque.getSize(), 1);
	TEST_ASSERT_TRUE(deque.pushBack(2));
	TEST_ASSERT_EQUALS(deque.getSize(), 2);
	TEST_ASSERT_TRUE(deque.pushBack(3));
	TEST_ASSERT_EQUALS(deque.getSize(), 3);
	
	TEST_ASSERT_FALSE(deque.pushBack(4));
	TEST_ASSERT_EQUALS(deque.getSize(), 3);
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.front(), 1);
	deque.popFront();
	TEST_ASSERT_EQUALS(deque.getSize(), 2);
	
	TEST_ASSERT_EQUALS(deque.front(), 2);
	deque.popFront();
	TEST_ASSERT_EQUALS(deque.getSize(), 1);
	
	TEST_ASSERT_TRUE(deque.pushBack(4));
	TEST_ASSERT_EQUALS(deque.getSize(), 2);
	TEST_ASSERT_TRUE(deque.pushBack(5));
	TEST_ASSERT_EQUALS(deque.getSize(), 3);
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.front(), 3);
	deque.popFront();
	TEST_ASSERT_EQUALS(deque.getSize(), 2);
	
	TEST_ASSERT_EQUALS(deque.front(), 4);
	deque.popFront();
	TEST_ASSERT_EQUALS(deque.getSize(), 1);
	
	TEST_ASSERT_EQUALS(deque.front(), 5);
	deque.popFront();
	
	TEST_ASSERT_EQUALS(deque.getSize(), 0);
	TEST_ASSERT_TRUE(deque.isEmpty());
}

void
BoundedDequeTest::testBackward()
{
	xpcc::BoundedDeque<int16_t, 3> deque;
	
	TEST_ASSERT_TRUE(deque.pushFront(1));
	TEST_ASSERT_TRUE(deque.pushFront(2));
	TEST_ASSERT_TRUE(deque.pushFront(3));
	
	TEST_ASSERT_FALSE(deque.pushFront(4));
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.back(), 1);
	deque.popBack();
	
	TEST_ASSERT_EQUALS(deque.back(), 2);
	deque.popBack();
	
	TEST_ASSERT_TRUE(deque.pushFront(4));
	TEST_ASSERT_TRUE(deque.pushFront(5));
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.back(), 3);
	deque.popBack();
	
	TEST_ASSERT_EQUALS(deque.back(), 4);
	deque.popBack();
	
	TEST_ASSERT_EQUALS(deque.back(), 5);
	deque.popBack();
	
	TEST_ASSERT_TRUE(deque.isEmpty());
}

void
BoundedDequeTest::testBoth()
{
	xpcc::BoundedDeque<int16_t, 3> deque;
	
	TEST_ASSERT_EQUALS(deque.getSize(), 0);
	
	TEST_ASSERT_TRUE(deque.pushFront(12));
	TEST_ASSERT_EQUALS(deque.getSize(), 1);
	
	TEST_ASSERT_TRUE(deque.pushFront(11));
	TEST_ASSERT_EQUALS(deque.getSize(), 2);
	
	TEST_ASSERT_TRUE(deque.pushFront(10));
	TEST_ASSERT_EQUALS(deque.getSize(), 3);
	
	TEST_ASSERT_EQUALS(deque.front(), 10);
	deque.popFront();
	
	TEST_ASSERT_EQUALS(deque.front(), 11);
	deque.popFront();
	
	TEST_ASSERT_TRUE(deque.pushBack(13));
	TEST_ASSERT_TRUE(deque.pushBack(14));
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.back(), 14);
	deque.popBack();
	
	TEST_ASSERT_EQUALS(deque.front(), 12);
	deque.popFront();
	
	TEST_ASSERT_EQUALS(deque.back(), 13);
	deque.popBack();
	
	TEST_ASSERT_TRUE(deque.isEmpty());
}

void
BoundedDequeTest::testClear()
{
	xpcc::BoundedDeque<int16_t, 3> deque;
	
	deque.pushFront(12);
	deque.pushFront(11);
	deque.pushFront(10);
	
	TEST_ASSERT_EQUALS(deque.getSize(), 3);
	
	deque.clear();
	
	TEST_ASSERT_EQUALS(deque.getSize(), 0);
	TEST_ASSERT_TRUE(deque.isEmpty());
	
	deque.pushFront(12);
	deque.pushFront(11);
	deque.pushFront(10);
	TEST_ASSERT_EQUALS(deque.getSize(), 3);
	
	TEST_ASSERT_EQUALS(deque.front(), 10);
	deque.popFront();
	
	TEST_ASSERT_EQUALS(deque.front(), 11);
	deque.popFront();
	
	deque.pushBack(13);
	deque.pushBack(14);
	TEST_ASSERT_TRUE(deque.isFull());
	
	TEST_ASSERT_EQUALS(deque.back(), 14);
	deque.popBack();
	
	TEST_ASSERT_EQUALS(deque.front(), 12);
	deque.popFront();
	
	TEST_ASSERT_EQUALS(deque.back(), 13);
	deque.popBack();
	
	TEST_ASSERT_TRUE(deque.isEmpty());
}

void
BoundedDequeTest::testConstIterator()
{
	xpcc::BoundedDeque<int16_t, 5> deque;
	
	deque.pushBack(3);
	deque.pushBack(4);
	deque.pushFront(2);
	deque.pushFront(1);
	
	xpcc::BoundedDeque<int16_t, 5>::const_iterator it;
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

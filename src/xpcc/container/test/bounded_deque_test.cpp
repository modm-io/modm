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
 */
// ----------------------------------------------------------------------------

#include <xpcc/container/deque.hpp>

#include "bounded_deque_test.hpp"

void
BoundedDequeTest::testForward()
{
	xpcc::BoundedDeque<int16_t, 3> deque;
	
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
	xpcc::BoundedDeque<int16_t, 3> deque;
	
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
	xpcc::BoundedDeque<int16_t, 3> deque;
	
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
	xpcc::BoundedDeque<int16_t, 3> deque;
	
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
	xpcc::BoundedDeque<int16_t, 5> deque;
	
	deque.append(3);
	deque.append(4);
	deque.prepend(2);
	deque.prepend(1);
	
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

void
BoundedDequeTest::testFull()
{
	xpcc::BoundedDeque<int16_t, 3> deque;

	TEST_ASSERT_TRUE(deque.append(3));
	TEST_ASSERT_TRUE(deque.append(4));
	TEST_ASSERT_TRUE(deque.append(5));
	TEST_ASSERT_FALSE(deque.append(6));
}

void
BoundedDequeTest::testOneElementQueue()
{
	xpcc::BoundedDeque<int16_t, 1> deque;

	TEST_ASSERT_EQUALS(deque.getSize(), 0);
	TEST_ASSERT_EQUALS(deque.getMaxSize(), 1);

	TEST_ASSERT_TRUE(deque.isEmpty());
	TEST_ASSERT_FALSE(deque.isFull());

	TEST_ASSERT_TRUE(deque.append(123));
	TEST_ASSERT_EQUALS(deque.getSize(), 1);

	TEST_ASSERT_FALSE(deque.isEmpty());
	TEST_ASSERT_TRUE(deque.isFull());

	TEST_ASSERT_FALSE(deque.append(2));

	TEST_ASSERT_EQUALS(deque.getFront(), 123);
	TEST_ASSERT_EQUALS(deque.getBack(), deque.getFront());

	deque.removeBack();
	TEST_ASSERT_TRUE(deque.isEmpty());
}

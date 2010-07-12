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

#include <unittest/type/count_type.hpp>
#include <xpcc/container/linked_list.hpp>

#include "linked_list_test.hpp"

void
LinkedListTest::setUp()
{
	// reset CounterType before every test
	unittest::CountType::reset();
}

void
LinkedListTest::testConstructor()
{
	xpcc::LinkedList< unittest::CountType > list;
	
	TEST_ASSERT_TRUE(list.isEmpty());
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
}

void
LinkedListTest::testAppend()
{
	xpcc::LinkedList<int16_t> list;
	
	list.append(1);
	
	TEST_ASSERT_FALSE(list.isEmpty());
	TEST_ASSERT_EQUALS(list.getFront(), 1);
	TEST_ASSERT_EQUALS(list.getBack(), 1);
	
	list.append(2);
	
	TEST_ASSERT_EQUALS(list.getFront(), 1);
	TEST_ASSERT_EQUALS(list.getBack(), 2);
	
	list.append(3);
	
	TEST_ASSERT_EQUALS(list.getFront(), 1);
	TEST_ASSERT_EQUALS(list.getBack(), 3);
}

void
LinkedListTest::testAppendCount()
{
	xpcc::LinkedList< unittest::CountType > list;
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 0U);
	
	unittest::CountType data;
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 0U);
	
	list.append(data);
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 1U);
	
	list.append(data);
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 2U);
}

void
LinkedListTest::testPrepend()
{
	xpcc::LinkedList<int16_t> list;
	
	list.prepend(1);
	
	TEST_ASSERT_FALSE(list.isEmpty());
	TEST_ASSERT_EQUALS(list.getFront(), 1);
	TEST_ASSERT_EQUALS(list.getBack(), 1);
	
	list.prepend(2);
	
	TEST_ASSERT_EQUALS(list.getFront(), 2);
	TEST_ASSERT_EQUALS(list.getBack(), 1);
	
	list.prepend(3);
	
	TEST_ASSERT_EQUALS(list.getFront(), 3);
	TEST_ASSERT_EQUALS(list.getBack(), 1);
}

void
LinkedListTest::testPrependCount()
{
	xpcc::LinkedList< unittest::CountType > list;
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 0U);
	
	unittest::CountType data;
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 0U);
	
	list.prepend(data);
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 1U);
	
	list.prepend(data);
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 2U);
}

void
LinkedListTest::testRemoveFront()
{
	xpcc::LinkedList<int16_t> list;
	
	list.append(1);
	list.append(2);
	list.append(3);
	
	TEST_ASSERT_EQUALS(list.getFront(), 1);
	
	list.removeFront();
	
	TEST_ASSERT_EQUALS(list.getFront(), 2);
	
	list.removeFront();
	
	TEST_ASSERT_EQUALS(list.getFront(), 3);
	TEST_ASSERT_FALSE(list.isEmpty());
	
	list.removeFront();
	
	TEST_ASSERT_TRUE(list.isEmpty());
}

void
LinkedListTest::testRemoveFrontCount()
{
	unittest::CountType data;
	
	xpcc::LinkedList< unittest::CountType > list;
	list.append(data);
	list.append(data);
	
	unittest::CountType::reset();
	
	list.removeFront();
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDestructorCalls, 1U);
}

void
LinkedListTest::testDestructor()
{
	unittest::CountType data;
	
	xpcc::LinkedList< unittest::CountType >* list = new xpcc::LinkedList< unittest::CountType >;
	
	list->append(data);
	list->append(data);
	list->append(data);
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 3U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDestructorCalls, 0U);
	
	unittest::CountType::reset();
	
	// call destructor
	delete list;
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDestructorCalls, 3U);
}

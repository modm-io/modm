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
 * $Id: linked_list_test.cpp 607 2011-09-13 19:51:03Z dergraaf $
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
	unittest::CountType::reset();
	
	xpcc::LinkedList< unittest::CountType >* list = new xpcc::LinkedList< unittest::CountType >;
	
	list->append(data);
	list->append(data);
	list->append(data);
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 3U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDestructorCalls, 0U);
	
	unittest::CountType::reset();
	
	// call destructor
	delete list;
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDestructorCalls, 3U);
}

namespace
{
	class IteratorTestClass
	{
	public:
		IteratorTestClass(uint8_t a, int16_t b) :
			a(a), b(b)
		{
		}
		
		uint8_t a;
		int16_t b;
	};
}

void
LinkedListTest::testConstIterator()
{
	xpcc::LinkedList<int16_t> writableList;
	const xpcc::LinkedList<int16_t>& list = writableList;
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	writableList.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	writableList.append(2);
	writableList.append(3);
	
	uint8_t i;
	xpcc::LinkedList<int16_t>::const_iterator it;
	
	it = list.begin();
	TEST_ASSERT_TRUE(it == list.begin());
	TEST_ASSERT_TRUE(it != list.end());
	
	++it;
	++it;
	
	TEST_ASSERT_TRUE(it != list.begin());
	TEST_ASSERT_TRUE(it != list.end());
	
	++it;
	
	TEST_ASSERT_TRUE(it != list.begin());
	TEST_ASSERT_TRUE(it == list.end());
	
	for (it = list.begin(), i = 1; it != list.end(); ++it, ++i) {
		TEST_ASSERT_EQUALS((*it), i); 
	}
}

void
LinkedListTest::testConstIteratorAccess()
{
	xpcc::LinkedList<IteratorTestClass> list;
	list.append(IteratorTestClass(12, -1532));
	
	xpcc::LinkedList<IteratorTestClass>::const_iterator it = list.begin();
	
	TEST_ASSERT_EQUALS(it->a, 12);
	TEST_ASSERT_EQUALS(it->b, -1532);
}

void
LinkedListTest::testIterator()
{
	xpcc::LinkedList<int16_t> list;
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	list.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	list.append(2);
	list.append(3);
	
	uint8_t i;
	xpcc::LinkedList<int16_t>::iterator it;
	
	it = list.begin();
	TEST_ASSERT_TRUE(it == list.begin());
	TEST_ASSERT_TRUE(it != list.end());
	
	++it;
	++it;
	
	TEST_ASSERT_TRUE(it != list.begin());
	TEST_ASSERT_TRUE(it != list.end());
	
	++it;
	
	TEST_ASSERT_TRUE(it != list.begin());
	TEST_ASSERT_TRUE(it == list.end());
	
	for (it = list.begin(), i = 1; it != list.end(); ++it, ++i) {
		TEST_ASSERT_EQUALS((*it), i); 
	}
}

void
LinkedListTest::testIteratorAccess()
{
	xpcc::LinkedList<IteratorTestClass> list;
	list.append(IteratorTestClass(12, -1532));
	
	xpcc::LinkedList<IteratorTestClass>::iterator it = list.begin();
	
	TEST_ASSERT_EQUALS(it->a, 12);
	TEST_ASSERT_EQUALS(it->b, -1532);
	
	it->a = 66;
	TEST_ASSERT_EQUALS(it->a, 66);
	
	(*it).b = 22312;
	TEST_ASSERT_EQUALS(it->b, 22312);
}

void
LinkedListTest::testSize()
{
	xpcc::LinkedList<int16_t> list;
	TEST_ASSERT_EQUALS(list.getSize(), 0U);
	
	list.append(1);
	TEST_ASSERT_EQUALS(list.getSize(), 1U);
	
	list.append(2);
	TEST_ASSERT_EQUALS(list.getSize(), 2U);
	
	list.prepend(3);
	TEST_ASSERT_EQUALS(list.getSize(), 3U);
	
	list.removeFront();
	TEST_ASSERT_EQUALS(list.getSize(), 2U);
	
	list.removeFront();
	TEST_ASSERT_EQUALS(list.getSize(), 1U);
	
	list.prepend(4);
	TEST_ASSERT_EQUALS(list.getSize(), 2U);
}

void
LinkedListTest::testRemove()
{
	xpcc::LinkedList<int16_t> list;
	xpcc::LinkedList<int16_t>::iterator it = list.begin();
	list.remove(it);
	
	TEST_ASSERT_TRUE(it == list.end());
	
	list.append(1);
	list.append(2);
	list.append(3);
	
	// [1, 2, 3]
	it = list.begin();
	it = list.remove(it);
	TEST_ASSERT_EQUALS(*it, 2);
	TEST_ASSERT_EQUALS(list.getFront(), 2);
	
	++it;
	++it;
	
	TEST_ASSERT_TRUE(it == list.end());	
	
	list.append(4);
	
	// [2, 3, 4]
	it = list.begin();
	++it;
	it = list.remove(it);
	TEST_ASSERT_EQUALS(*it, 4);
	TEST_ASSERT_EQUALS(list.getFront(), 2);
	TEST_ASSERT_EQUALS(list.getBack(), 4);
	
	// [2, 4]
	it = list.begin();
	++it;
	it = list.remove(it);
	TEST_ASSERT_TRUE(it == list.end());
	
	TEST_ASSERT_EQUALS(list.getFront(), 2);
	TEST_ASSERT_EQUALS(list.getBack(), 2);
}

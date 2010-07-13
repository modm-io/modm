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
#include <xpcc/container/doubly_linked_list.hpp>

#include "doubly_linked_list_test.hpp"

void
DoublyLinkedListTest::setUp()
{
	// reset CounterType before every test
	unittest::CountType::reset();
}

void
DoublyLinkedListTest::testConstructor()
{
	xpcc::DoublyLinkedList< unittest::CountType > list;
	
	TEST_ASSERT_TRUE(list.isEmpty());
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
}

void
DoublyLinkedListTest::testAppend()
{
	xpcc::DoublyLinkedList<int16_t> list;
	
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
DoublyLinkedListTest::testAppendCount()
{
	xpcc::DoublyLinkedList< unittest::CountType > list;
	
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
DoublyLinkedListTest::testPrepend()
{
	xpcc::DoublyLinkedList<int16_t> list;
	
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
DoublyLinkedListTest::testPrependCount()
{
	xpcc::DoublyLinkedList< unittest::CountType > list;
	
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
DoublyLinkedListTest::testRemoveFront()
{
	xpcc::DoublyLinkedList<int16_t> list;
	
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
DoublyLinkedListTest::testRemoveFrontCount()
{
	unittest::CountType data;
	
	xpcc::DoublyLinkedList< unittest::CountType > list;
	list.append(data);
	list.append(data);
	
	unittest::CountType::reset();
	
	list.removeFront();
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDestructorCalls, 1U);
}

void
DoublyLinkedListTest::testRemoveBack()
{
	xpcc::DoublyLinkedList<int16_t> list;
	
	list.append(1);
	list.append(2);
	list.append(3);
	
	TEST_ASSERT_EQUALS(list.getBack(), 3);
	
	list.removeBack();
	
	TEST_ASSERT_EQUALS(list.getBack(), 2);
	
	list.removeBack();
	
	TEST_ASSERT_EQUALS(list.getBack(), 1);
	TEST_ASSERT_FALSE(list.isEmpty());
	
	list.removeBack();
	
	TEST_ASSERT_TRUE(list.isEmpty());
}

void
DoublyLinkedListTest::testRemoveBackCount()
{
	unittest::CountType data;
	
	xpcc::DoublyLinkedList< unittest::CountType > list;
	list.append(data);
	list.append(data);
	
	unittest::CountType::reset();
	
	list.removeBack();
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDestructorCalls, 1U);
}

void
DoublyLinkedListTest::testDestructor()
{
	unittest::CountType data;
	
	xpcc::DoublyLinkedList< unittest::CountType >* list = new xpcc::DoublyLinkedList< unittest::CountType >;
	
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
DoublyLinkedListTest::testConstIterator()
{
	xpcc::DoublyLinkedList<int16_t> writableList;
	const xpcc::DoublyLinkedList<int16_t>& list = writableList;
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	writableList.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	writableList.append(2);
	writableList.append(3);
	
	uint8_t i;
	xpcc::DoublyLinkedList<int16_t>::const_iterator it;
	
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
DoublyLinkedListTest::testConstIteratorAccess()
{
	xpcc::DoublyLinkedList<IteratorTestClass> list;
	list.append(IteratorTestClass(12, -1532));
	
	xpcc::DoublyLinkedList<IteratorTestClass>::const_iterator it = list.begin();
	
	TEST_ASSERT_EQUALS(it->a, 12);
	TEST_ASSERT_EQUALS(it->b, -1532);
}

void
DoublyLinkedListTest::testIterator()
{
	xpcc::DoublyLinkedList<int16_t> list;
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	list.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	list.append(2);
	list.append(3);
	
	uint8_t i;
	xpcc::DoublyLinkedList<int16_t>::iterator it;
	
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
DoublyLinkedListTest::testIteratorAccess()
{
	xpcc::DoublyLinkedList<IteratorTestClass> list;
	list.append(IteratorTestClass(12, -1532));
	
	xpcc::DoublyLinkedList<IteratorTestClass>::iterator it = list.begin();
	
	TEST_ASSERT_EQUALS(it->a, 12);
	TEST_ASSERT_EQUALS(it->b, -1532);
	
	it->a = 66;
	TEST_ASSERT_EQUALS(it->a, 66);
	
	(*it).b = 22312;
	TEST_ASSERT_EQUALS(it->b, 22312);
}

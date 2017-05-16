/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/type/count_type.hpp>
#include <modm/container/linked_list.hpp>

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
	modm::LinkedList< unittest::CountType > list;
	
	TEST_ASSERT_TRUE(list.isEmpty());
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
}

void
LinkedListTest::testAppend()
{
	modm::LinkedList<int16_t> list;
	
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
	modm::LinkedList< unittest::CountType > list;
	
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
	modm::LinkedList<int16_t> list;
	
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
	modm::LinkedList< unittest::CountType > list;
	
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
	modm::LinkedList<int16_t> list;
	
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
	
	modm::LinkedList< unittest::CountType > list;
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
	
	modm::LinkedList< unittest::CountType >* list = new modm::LinkedList< unittest::CountType >;
	
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
	modm::LinkedList<int16_t> writableList;
	const modm::LinkedList<int16_t>& list = writableList;
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	writableList.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	writableList.append(2);
	writableList.append(3);
	
	uint8_t i;
	modm::LinkedList<int16_t>::const_iterator it;
	
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
	modm::LinkedList<IteratorTestClass> list;
	list.append(IteratorTestClass(12, -1532));
	
	modm::LinkedList<IteratorTestClass>::const_iterator it = list.begin();
	
	TEST_ASSERT_EQUALS(it->a, 12);
	TEST_ASSERT_EQUALS(it->b, -1532);
}

void
LinkedListTest::testIterator()
{
	modm::LinkedList<int16_t> list;
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	list.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	list.append(2);
	list.append(3);
	
	uint8_t i;
	modm::LinkedList<int16_t>::iterator it;
	
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
	modm::LinkedList<IteratorTestClass> list;
	list.append(IteratorTestClass(12, -1532));
	
	modm::LinkedList<IteratorTestClass>::iterator it = list.begin();
	
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
	modm::LinkedList<int16_t> list;
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
	modm::LinkedList<int16_t> list;
	modm::LinkedList<int16_t>::iterator it = list.begin();
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

void
LinkedListTest::testInsert()
{
	modm::LinkedList<int16_t> list;
	modm::LinkedList<int16_t>::iterator it = list.begin();
	list.insert(it, 0);
	TEST_ASSERT_FALSE(list.isEmpty());
	list.insert(it, 2);
	list.insert(it, 4);

	TEST_ASSERT_EQUALS(list.getSize(), 3u);

	int16_t ii = 0;
	for(const auto value : list) {
		TEST_ASSERT_EQUALS(value, ii);
		ii += 2;
	}

	it = list.begin();
	list.insert(it, 1);
	++it; ++it;
	list.insert(it, 3);
	++it; ++it;
	list.insert(it, 5);

	ii = 0;
	for(const auto value : list) {
		TEST_ASSERT_EQUALS(value, ii);
		ii += 1;
	}
}

/*
 * Copyright (c) 2010, Fabian Greif
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
#include <modm/container/dynamic_array.hpp>

#include "dynamic_array_test.hpp"

typedef modm::DynamicArray<int16_t> Container;

void
DynamicArrayTest::setUp()
{
	unittest::CountType::reset();
}

void
DynamicArrayTest::testDefaultConstrutor()
{
	Container array;

	TEST_ASSERT_TRUE(array.isEmpty());
	TEST_ASSERT_EQUALS(array.getSize(), 0U);
}

void
DynamicArrayTest::testAllocationConstructor()
{
	Container array(10);

	TEST_ASSERT_TRUE(array.isEmpty());
	TEST_ASSERT_EQUALS(array.getSize(), 0U);
	TEST_ASSERT_TRUE(array.getCapacity() >= 10);
}

void
DynamicArrayTest::testSequenceConstructor()
{
	Container array(10, 123);

	TEST_ASSERT_FALSE(array.isEmpty());
	TEST_ASSERT_EQUALS(array.getSize(), 10U);

	for (uint_fast16_t i = 0; i < array.getSize(); ++i) {
		TEST_ASSERT_EQUALS(array[i], 123);
	}
}

void
DynamicArrayTest::testCopyConstructor()
{
	Container array(4, 123);

	Container array2(array);

	TEST_ASSERT_FALSE(array2.isEmpty());
	TEST_ASSERT_EQUALS(array2.getSize(), 4U);

	for (uint_fast16_t i = 0; i < array2.getSize(); ++i) {
		TEST_ASSERT_EQUALS(array2[i], 123);
	}

	array[0] = 1;

	TEST_ASSERT_EQUALS(array[0], 1);
	TEST_ASSERT_EQUALS(array2[0], 123);
}

void
DynamicArrayTest::testInitializerListConstructor()
{
	Container array{1, 2, 3, 4, 5};

	TEST_ASSERT_FALSE(array.isEmpty());
	TEST_ASSERT_EQUALS(array.getSize(), 5U);
	TEST_ASSERT_EQUALS(array[0], 1);
	TEST_ASSERT_EQUALS(array[1], 2);
	TEST_ASSERT_EQUALS(array[2], 3);
	TEST_ASSERT_EQUALS(array[3], 4);
	TEST_ASSERT_EQUALS(array[4], 5);
}

void
DynamicArrayTest::testReserve()
{
	Container array;
	array.reserve(10);

	TEST_ASSERT_TRUE(array.isEmpty());
	TEST_ASSERT_EQUALS(array.getSize(), 0U);
	TEST_ASSERT_TRUE(array.getCapacity() >= 10);
}

void
DynamicArrayTest::testAppend()
{
	Container array;

	array.append(20);

	TEST_ASSERT_FALSE(array.isEmpty());
	TEST_ASSERT_EQUALS(array.getSize(), 1U);

	TEST_ASSERT_EQUALS(array[0], 20);

	array.append(5);

	TEST_ASSERT_EQUALS(array.getSize(), 2U);

	TEST_ASSERT_EQUALS(array[0], 20);
	TEST_ASSERT_EQUALS(array[1], 5);
}

void
DynamicArrayTest::testRemove()
{
	Container array(2, 5);

	TEST_ASSERT_FALSE(array.isEmpty());
	TEST_ASSERT_EQUALS(array.getSize(), 2U);

	array.removeBack();

	TEST_ASSERT_FALSE(array.isEmpty());
	TEST_ASSERT_EQUALS(array.getSize(), 1U);

	array.removeBack();

	TEST_ASSERT_TRUE(array.isEmpty());
	TEST_ASSERT_EQUALS(array.getSize(), 0U);
}

void
DynamicArrayTest::testClear()
{
	modm::DynamicArray<unittest::CountType> array(5);
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 0U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfOperations, 0U);
	
	unittest::CountType::reset();
	
	unittest::CountType data;
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfOperations, 1U);
	
	unittest::CountType::reset();
	
	array.append(data);
	array.append(data);
	array.append(data);
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 3U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfOperations, 3U);
	
	array.clear();
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDestructorCalls, 3U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfOperations, 6U);
	
	TEST_ASSERT_EQUALS(array.getSize(), 0U);
	TEST_ASSERT_EQUALS(array.getCapacity(), 0U);
}

void
DynamicArrayTest::testRemoveAll()
{
	modm::DynamicArray<unittest::CountType> array(5);
	
	unittest::CountType data;
	
	array.append(data);
	array.append(data);
	array.append(data);
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfCopyConstructorCalls, 3U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfOperations, 4U);
	
	unittest::CountType::reset();
	
	array.removeAll();
	
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfDestructorCalls, 3U);
	TEST_ASSERT_EQUALS(unittest::CountType::numberOfOperations, 3U);
	
	TEST_ASSERT_EQUALS(array.getSize(), 0U);
	TEST_ASSERT_EQUALS(array.getCapacity(), 5U);
}

// ----------------------------------------------------------------------------

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
DynamicArrayTest::testConstIterator()
{
	modm::DynamicArray<int16_t> writableList(5);
	const modm::DynamicArray<int16_t>& list = writableList;
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	writableList.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	writableList.append(2);
	writableList.append(3);
	
	uint8_t i;
	modm::DynamicArray<int16_t>::const_iterator it;
	
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
DynamicArrayTest::testConstIteratorAccess()
{
	modm::DynamicArray<IteratorTestClass> list(2);
	list.append(IteratorTestClass(12, -1532));
	
	modm::DynamicArray<IteratorTestClass>::const_iterator it = list.begin();
	
	TEST_ASSERT_EQUALS(it->a, 12);
	TEST_ASSERT_EQUALS(it->b, -1532);
}

void
DynamicArrayTest::testIterator()
{
	modm::DynamicArray<int16_t> list(5);
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	list.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	list.append(2);
	list.append(3);
	
	uint8_t i;
	modm::DynamicArray<int16_t>::iterator it;
	
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
DynamicArrayTest::testIteratorAccess()
{
	modm::DynamicArray<IteratorTestClass> list(2);
	list.append(IteratorTestClass(12, -1532));
	
	modm::DynamicArray<IteratorTestClass>::iterator it = list.begin();
	
	TEST_ASSERT_EQUALS(it->a, 12);
	TEST_ASSERT_EQUALS(it->b, -1532);
	
	it->a = 66;
	TEST_ASSERT_EQUALS(it->a, 66);
	
	(*it).b = 22312;
	TEST_ASSERT_EQUALS(it->b, 22312);
}

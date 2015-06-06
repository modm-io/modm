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

#include <unittest/type/count_type.hpp>
#include <xpcc/container/dynamic_array.hpp>

#include "dynamic_array_test.hpp"

typedef xpcc::DynamicArray<int16_t> Container;

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
	xpcc::DynamicArray<unittest::CountType> array(5);
	
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
	xpcc::DynamicArray<unittest::CountType> array(5);
	
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
	xpcc::DynamicArray<int16_t> writableList(5);
	const xpcc::DynamicArray<int16_t>& list = writableList;
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	writableList.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	writableList.append(2);
	writableList.append(3);
	
	uint8_t i;
	xpcc::DynamicArray<int16_t>::const_iterator it;
	
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
	xpcc::DynamicArray<IteratorTestClass> list(2);
	list.append(IteratorTestClass(12, -1532));
	
	xpcc::DynamicArray<IteratorTestClass>::const_iterator it = list.begin();
	
	TEST_ASSERT_EQUALS(it->a, 12);
	TEST_ASSERT_EQUALS(it->b, -1532);
}

void
DynamicArrayTest::testIterator()
{
	xpcc::DynamicArray<int16_t> list(5);
	
	TEST_ASSERT_TRUE(list.begin() == list.end());
	TEST_ASSERT_FALSE(list.begin() != list.end());
	
	list.append(1);
	
	TEST_ASSERT_FALSE(list.begin() == list.end());
	TEST_ASSERT_TRUE(list.begin() != list.end());
	
	list.append(2);
	list.append(3);
	
	uint8_t i;
	xpcc::DynamicArray<int16_t>::iterator it;
	
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
	xpcc::DynamicArray<IteratorTestClass> list(2);
	list.append(IteratorTestClass(12, -1532));
	
	xpcc::DynamicArray<IteratorTestClass>::iterator it = list.begin();
	
	TEST_ASSERT_EQUALS(it->a, 12);
	TEST_ASSERT_EQUALS(it->b, -1532);
	
	it->a = 66;
	TEST_ASSERT_EQUALS(it->a, 66);
	
	(*it).b = 22312;
	TEST_ASSERT_EQUALS(it->b, 22312);
}

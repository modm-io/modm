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

#include <unittest/types/counter_type.hpp>
#include <xpcc/container/dynamic_array.hpp>

#include "dynamic_array_test.hpp"

typedef xpcc::DynamicArray<int16_t> Container;

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
	unittest::CounterType::reset();
	
	xpcc::DynamicArray<unittest::CounterType> array(5);
	
	TEST_ASSERT_EQUALS(unittest::CounterType::numberOfDefaultConstructorCalls, 5U);
	
	unittest::CounterType::reset();
	unittest::CounterType data;
	
	array.append(data);
	array.append(data);
	array.append(data);
	
	TEST_ASSERT_EQUALS(unittest::CounterType::numberOfAssignments +
			unittest::CounterType::numberOfCopyConstructorCalls, 3U);
	TEST_ASSERT_EQUALS(unittest::CounterType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CounterType::numberOfDestructorCalls, 0U);
	
	array.clear();
	
	TEST_ASSERT_EQUALS(unittest::CounterType::numberOfDestructorCalls, 5U);
	
	TEST_ASSERT_EQUALS(array.getSize(), 0U);
	TEST_ASSERT_EQUALS(array.getCapacity(), 0U);
}

void
DynamicArrayTest::testRemoveAll()
{
	xpcc::DynamicArray<unittest::CounterType> array(5);
	
	unittest::CounterType::reset();
	unittest::CounterType data;
	
	array.append(data);
	array.append(data);
	array.append(data);
	
	TEST_ASSERT_EQUALS(unittest::CounterType::numberOfAssignments +
			unittest::CounterType::numberOfCopyConstructorCalls, 3U);
	TEST_ASSERT_EQUALS(unittest::CounterType::numberOfDefaultConstructorCalls, 1U);
	TEST_ASSERT_EQUALS(unittest::CounterType::numberOfDestructorCalls, 0U);
	
	array.removeAll();
	
	TEST_ASSERT_EQUALS(unittest::CounterType::numberOfDestructorCalls, 3U);
	
	TEST_ASSERT_EQUALS(array.getSize(), 0U);
	TEST_ASSERT_EQUALS(array.getCapacity(), 5U);
}

/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "block_allocator_test.hpp"

#include "../block_allocator.hpp"

void
BlockAllocatorTest::testAvailableSize()
{
	uint8_t *heap = new uint8_t[512];
	
	xpcc::BlockAllocator<uint16_t, 8> allocator;
	allocator.initialize(heap, heap + 512);
	
	TEST_ASSERT_EQUALS(allocator.getAvailableSize(), 496U);
	
	delete[] heap;
}

void
BlockAllocatorTest::testAllocate()
{
	uint8_t *heap = new uint8_t[512];
	
	xpcc::BlockAllocator<uint16_t, 8> allocator;
	allocator.initialize(heap, heap + 512);
	
	TEST_ASSERT_EQUALS(allocator.getAvailableSize(), 496U);
	
	allocator.allocate(12);
	
	TEST_ASSERT_EQUALS(allocator.getAvailableSize(), 480U);
	
	allocator.allocate(13);
	
	TEST_ASSERT_EQUALS(allocator.getAvailableSize(), 448U);
	
	allocator.allocate(440);
	
	TEST_ASSERT_EQUALS(allocator.getAvailableSize(), 0U);
	TEST_ASSERT_EQUALS(allocator.allocate(1), (void *) 0);
	
	delete[] heap;
}

void
BlockAllocatorTest::testFree()
{
	uint8_t *heap = new uint8_t[512];
	
	xpcc::BlockAllocator<uint16_t, 8> allocator;
	allocator.initialize(heap, heap + 512);
	
	TEST_ASSERT_EQUALS(allocator.getAvailableSize(), 496U);
	
	void* firstBlock = allocator.allocate(12);
	void* secondBlock = allocator.allocate(12);
	
	TEST_ASSERT_FALSE(firstBlock == secondBlock);
	
	allocator.free(firstBlock);
	
	void *thirdBlock = allocator.allocate(12);
	
	TEST_ASSERT_TRUE(firstBlock == thirdBlock);
	
	delete[] heap;
}

void
BlockAllocatorTest::testAlignment()
{
	uint8_t *heap = new uint8_t[512];

	for (uint_fast8_t misalignment = 0; misalignment < 6; ++misalignment)
	{
		xpcc::BlockAllocator<uint16_t, 8> allocator;
		allocator.initialize(heap + misalignment, heap + 512);

		TEST_ASSERT_EQUALS(allocator.getAvailableSize(), 496U);

		void* firstBlock = allocator.allocate(12);
		void* secondBlock = allocator.allocate(12);

		TEST_ASSERT_EQUALS( ((uintptr_t)firstBlock) % 4, 0U);
		TEST_ASSERT_EQUALS( ((uintptr_t)secondBlock) % 4, 0U);
	}

	delete[] heap;
}

// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

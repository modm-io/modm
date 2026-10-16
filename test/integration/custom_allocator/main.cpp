/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

#include "../integration_test.hpp"

/**
 * Tests that a custom allocator replaces the one of modm: all memory must come
 * from our own heap, until it is exhausted.
 */

using namespace Board;

// Allocate giant array inside the SRAM1 noinit section
// Play around with the array size and see effect it has on HeapTable!
modm_section(".noinit_sram1")
uint8_t heap_begin[30*1024]; // 31kB overflows the linkerscript
const uint8_t *const heap_end{heap_begin + sizeof(heap_begin)};
const uint8_t *heap_top{heap_begin};

extern "C" void __modm_initialize_memory()
{
    // Initialize your specific allocator algorithm here
    memset(heap_begin, 0xaa, sizeof(heap_begin));
}
size_t allocations{0};

extern "C" modm_used void* _sbrk_r(struct _reent *,  ptrdiff_t size)
{
	const uint8_t *const heap = heap_top;
	heap_top += size;
	if (heap_top >= heap_end) {
		MODM_LOG_INFO << "Heap overflowed after " << allocations << "kB!" << modm::endl;
		// 30kB heap: the allocator has some overhead per allocation
		finishTest(allocations >= 25 and allocations <= 30);
	}
	return (void*) heap;
}
extern "C" void operator_delete(void* ptr)
{
	free(ptr);
}

// ----------------------------------------------------------------------------
int main()
{
	Board::initialize();

	for (const auto [traits, start, end, size] : modm::platform::HeapTable())
	{
		MODM_LOG_INFO.printf("Memory section %#x @[0x%p,0x%p](%u)\n",
							 traits.value, start, end, size);
	}

	// leak memory until our heap is exhausted
	while (allocations < 100)
	{
		const uint8_t *const ptr = new uint8_t[1024];
		// all memory must be inside of our heap
		if (ptr < heap_begin or ptr >= heap_end) break;
		allocations++;
	}

	// the heap never overflowed or returned foreign memory
	return finishTest(false);
}

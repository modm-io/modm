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
extern "C" void* _sbrk_r(struct _reent *,  ptrdiff_t size)
{
    const uint8_t *const heap = heap_top;
    heap_top += size;
    if (heap_top >= heap_end) {
    	MODM_LOG_ERROR << "Heap overflowed!" << modm::endl;
    	while(1) ;
    }
    return (void*) heap;
}

// ----------------------------------------------------------------------------
int main()
{
	Board::initialize();
	LedD13::setOutput();

	for (const auto [traits, start, end, size] : modm::platform::HeapTable())
	{
		MODM_LOG_INFO.printf("Memory section %#x @[0x%p,0x%p](%u)\n",
							 traits.value, start, end, size);
	}

	void* ptr;
	size_t counter{0};
	while (true)
	{
		LedD13::toggle();
		modm::delay(200ms);

		// leak memory until heap is exhausted
		ptr = malloc(1024);
		if (ptr) counter++;
		MODM_LOG_INFO << "Allocated " << counter << "kb of heap!" << modm::endl;
	}

	return 0;
}

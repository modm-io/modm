/*
 * Copyright (c) 2016, 2019 Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdint.h>
#include <reent.h>
#include <modm/architecture/interface/assert.h>
#include <modm/platform/core/heap_table.hpp>

// ----------------------------------------------------------------------------
extern "C"
{

const uint8_t *heap_top{nullptr};
const uint8_t *heap_end{nullptr};

void __modm_initialize_memory(void)
{
	// find the largest heap that is DMA-able and S-Bus accessible
	bool success = modm::platform::HeapTable::find_largest(&heap_top, &heap_end);
	modm_assert(success, "heap.init", "Could not find main heap memory!");
}

/* Support function. Adjusts end of heap to provide more memory to
 * memory allocator.
 *
 *  struct _reent *r -- re-entrancy structure, used by newlib to
 *                      support multiple threads of operation.
 *  ptrdiff_t size   -- number of bytes to add.
 *                      Returns pointer to start of new heap area.
 *
 *  Note:  This implementation is not thread safe (despite taking a
 *         _reent structure as a parameter).
 */
void *
_sbrk_r(struct _reent *,  ptrdiff_t size)
{
	const uint8_t *const heap = heap_top;
	heap_top += size;
	modm_assert(heap_top < heap_end, "heap.sbrk", "Heap overflowed!", size);
	return (void*) heap;
}

}

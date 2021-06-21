/*
 * Copyright (c) 2016, Niklas Hauser
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
#include <string.h>
#include <reent.h>
#include <errno.h>
#include <modm/architecture/interface/assert.hpp>
#include <modm/platform/core/heap_table.hpp>

// ----------------------------------------------------------------------------
// Using the MODM Block Allocator
#include <modm/driver/storage/block_allocator.hpp>

#ifndef MODM_MEMORY_BLOCK_ALLOCATOR_TYPE
#define MODM_MEMORY_BLOCK_ALLOCATOR_TYPE uint16_t
#endif

#ifndef MODM_MEMORY_BLOCK_ALLOCATOR_CHUNK_SIZE
#define MODM_MEMORY_BLOCK_ALLOCATOR_CHUNK_SIZE 8
#endif

static modm::BlockAllocator< MODM_MEMORY_BLOCK_ALLOCATOR_TYPE, MODM_MEMORY_BLOCK_ALLOCATOR_CHUNK_SIZE >
	allocator;
// this allocator has a maximum heap size!
const size_t max_heap_size = (1 << (sizeof(MODM_MEMORY_BLOCK_ALLOCATOR_TYPE) * 8)) *
							  MODM_MEMORY_BLOCK_ALLOCATOR_CHUNK_SIZE;

extern "C"
{

void __modm_initialize_memory(void)
{
	const uint8_t *heap_start{nullptr}, *heap_end{nullptr};
	// find the largest heap that is DMA-able and S-Bus accessible
	bool success = modm::platform::HeapTable::find_largest(&heap_start, &heap_end);
	modm_assert(success, "heap.init", "Could not find main heap memory!");
	// clamp the heap size to the maximum
	if (size_t(heap_end - heap_start) > max_heap_size) {
		heap_end = heap_start + max_heap_size;
	}
	// initialize the heap
	allocator.initialize((void*)heap_start, (void*)heap_end);
}

extern void __malloc_lock(struct _reent *);
extern void __malloc_unlock(struct _reent *);

void* __wrap__malloc_r(struct _reent *r, size_t size)
{
	__malloc_lock(r);
	void *ptr = allocator.allocate(size);
	__malloc_unlock(r);
	modm_assert_continue_fail_debug(ptr, "malloc",
			"No memory left in Block heap!", size);
	return ptr;
}

void* __wrap__calloc_r(struct _reent *r, size_t size)
{
	void *ptr = __wrap__malloc_r(r, size);
	if (ptr) memset(ptr, 0, size);
	return ptr;
}

void* __wrap__realloc_r(struct _reent *r, void *, size_t size)
{
	if (!p) return __wrap__malloc_r(r, size);
	// NOT IMPLEMENTED!
	modm_assert(0, "realloc", "Realloc is not implemented for Block heap!", size);
	return NULL;
}

void __wrap__free_r(struct _reent *r, void *p)
{
	__malloc_lock(r);
	allocator.free(p);
	__malloc_unlock(r);
}

} // extern "C"

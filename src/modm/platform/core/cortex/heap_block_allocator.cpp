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
#include "heap_table.hpp"

// ----------------------------------------------------------------------------
// Using the MODM Block Allocator
#include <modm/architecture/driver/heap/block_allocator.hpp>

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
	const uint32_t *heap_start, *heap_end;
	// find the largest heap that is DMA-able and S-Bus accessible
	modm::platform::HeapTable::find_largest(&heap_start, &heap_end);
	modm_assert(heap_start, "core", "heap", "init");
	// clamp the heap size to the maximum
	if ((size_t) heap_end - (size_t) heap_start > max_heap_size) {
		heap_end = (const uint32_t *) ((char *) heap_start + max_heap_size);
	}
	// initialize the heap
	allocator.initialize((void*)heap_start, (void*)heap_end);
}

void *__wrap__malloc_r(struct _reent *r, size_t size)
{
	(void) r;
	void *ptr = allocator.allocate(size);
	modm_assert_debug(ptr, "core", "heap", "malloc", size);
	return ptr;
}

void *__wrap__calloc_r(struct _reent *r, size_t size)
{
	void *ptr = __wrap__malloc_r(r, size);
	if (modm_assert_debug(ptr, "core", "heap", "calloc", size)) {
		memset(ptr, 0, size);
	}
	return ptr;
}

void *__wrap__realloc_r(struct _reent *r, void *p, size_t size)
{
	(void) r;
	(void) p;
	(void) size;
	// NOT IMPLEMENTED!
	modm_assert_debug(0, "core", "heap", "realloc", size);
	return NULL;
}

void __wrap__free_r(struct _reent *r, void *p)
{
	(void) r;
	allocator.free(p);
}

// _sbrk_r is empty
void *
_sbrk_r(struct _reent *r,  ptrdiff_t size)
{
	(void) r;
	(void) size;
	return NULL;
}

// memory traits are ignored for newlib allocator
void *malloc_tr(size_t size, uint32_t traits)
{
	(void) traits;
	return malloc(size);
}

} // extern "C"

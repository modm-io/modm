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
#include <string.h>
#include <reent.h>
#include <errno.h>
#include <modm/architecture/interface/assert.h>
#include "heap_table.hpp"

// ----------------------------------------------------------------------------
extern "C"
{

const uint8_t *__brkval = 0;
const uint8_t *heap_end = 0;

void __modm_initialize_memory(void)
{
	// find the largest heap that is DMA-able and S-Bus accessible
	modm::platform::HeapTable::find_largest(
		(const uint32_t **) &__brkval, (const uint32_t **) &heap_end);
	modm_assert(__brkval, "core", "heap", "init");
}

/* Support function. Adjusts end of heap to provide more memory to
 * memory allocator. Simple and dumb with no sanity checks.
 *
 *  struct _reent *r -- re-entrancy structure, used by newlib to
 *                      support multiple threads of operation.
 *  ptrdiff_t size   -- number of bytes to add.
 *                      Returns pointer to start of new heap area.
 *
 *  Note:  This implementation is not thread safe (despite taking a
 *         _reent structure as a parameter).
 *         Since the _reent struct is not used in the current implementation,
 *         the following messages must be suppressed.
 */
void *
_sbrk_r(struct _reent *r,  ptrdiff_t size)
{
	(void) r;
	// move heap pointer
	const uint8_t *heap = __brkval;
	__brkval += size;

	modm_assert(__brkval < heap_end, "core", "heap", "sbrk", size);

	//  Return pointer to start of new heap area.
	return (void*)heap;
}
void *__real__malloc_r(struct _reent *r, size_t size);
void *__wrap__malloc_r(struct _reent *r, size_t size) {
	void * ptr = __real__malloc_r(r, size);
	modm_assert_debug(ptr, "core", "heap", "malloc", size);
	return ptr;
}
void *__real__calloc_r(struct _reent *r, size_t size);
void *__wrap__calloc_r(struct _reent *r, size_t size) {
	void * ptr = __real__calloc_r(r, size);
	modm_assert_debug(ptr, "core", "heap", "calloc", size);
	return ptr;
}
void *__real__realloc_r(struct _reent *r, void *p, size_t size);
void *__wrap__realloc_r(struct _reent *r, void *p, size_t size) {
	void * ptr = __real__realloc_r(r, p, size);
	modm_assert_debug(ptr, "core", "heap", "realloc", size);
	return ptr;
}
void __real__free_r(struct _reent *r, void *p);
void __wrap__free_r(struct _reent *r, void *p) {
	__real__free_r(r, p);
}

// memory traits are ignored for newlib allocator
void *malloc_tr(size_t size, uint32_t traits)
{
	(void) traits;
	return malloc(size);
}

}

/*
 * Copyright (c) 2016-2017, Niklas Hauser
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
#include <tlsf/tlsf.h>

#ifndef MODM_TLSF_MAX_MEM_POOL_COUNT
#define MODM_TLSF_MAX_MEM_POOL_COUNT 6
#endif

typedef struct
{
	uint16_t traits;
	tlsf_t tlsf;
	const uint32_t* end;
} mem_pool_t;

static mem_pool_t mem_pools[MODM_TLSF_MAX_MEM_POOL_COUNT];

extern "C"
{

void
__modm_initialize_memory(void)
{
	uint16_t current_traits = 0;
	mem_pool_t *current_pool = mem_pools;

	// iterate over the entire table but not longer than mem_pools
	for (const auto [ttraits, tstart, tend, tsize] : modm::platform::HeapTable())
	{
		if (current_pool >= (mem_pools + MODM_TLSF_MAX_MEM_POOL_COUNT))
			break;

		// if the pool has a new trait than the previous one, or
		// if the pool has the same trait, but is non-continous
		if ((ttraits.value != current_traits) || (tstart != (current_pool - 1)->end))
		{
			if (tlsf_t pool = tlsf_create_with_pool((void*)tstart, tsize); pool) {
				// create a new allocator in this pool with these new traits.
				current_traits = ttraits.value;
				current_pool->traits = current_traits;
				current_pool->tlsf = pool;
				current_pool->end = tend;

				current_pool++;
			}
		}
		// otherwise add this pool to the existing allocator
		else if (tlsf_add_pool((current_pool - 1)->tlsf, (void*)tstart, tsize)) {
			(current_pool - 1)->end = tend;
		}
	}
}

static tlsf_t
get_tlsf_for_ptr(void *p)
{
	for (mem_pool_t *pool = mem_pools;
		 pool < (mem_pools + MODM_TLSF_MAX_MEM_POOL_COUNT);
		 pool++)
	{
		if ((pool->tlsf < p) && (p < (void *) pool->end))
		{
			// pointer is within this pool
			return pool->tlsf;
		}
	}
	modm_assert_debug(0, "core", "heap", "pool");
	return NULL;
}

void * malloc_tr(size_t size, uint32_t traits)
{
try_again:
	for (mem_pool_t *pool = mem_pools;
		 pool < (mem_pools + MODM_TLSF_MAX_MEM_POOL_COUNT);
		 pool++)
	{
		if ((pool->traits & traits) == traits)
		{
			void *p = tlsf_malloc(pool->tlsf, size);
			if (p) return p;
		}
	}

	// clear the types core coupled and external, but not non-volatile
	const uint32_t clear_mask = 0x8000 | 0x2000;
	if (traits & clear_mask) {
		traits &= ~clear_mask;
		goto try_again;
	}
	// set the SBus and try again
	const uint32_t set_mask = 0x0001;
	if (!(traits & set_mask)) {
		traits |= set_mask;
		goto try_again;
	}
	// there is no memory left even after fallback.
	modm_assert_debug(0, "core", "heap", "malloc", size);
	return NULL;
}

void *__wrap__malloc_r(struct _reent *r, size_t size)
{
	(void) r;
	// default is accessible by S-Bus and DMA-able
	return malloc_tr(size, 0x8 | 0x1);
}

void *__wrap__calloc_r(struct _reent *r, size_t size)
{
	(void) r;
	void *ptr = __wrap__malloc_r(r, size);
	if (ptr) memset(ptr, 0, size);
	return ptr;
}

void *__wrap__realloc_r(struct _reent *r, void *p, size_t size)
{
	(void) r;
	tlsf_t pool = get_tlsf_for_ptr(p);
	// if pointer belongs to no pool, exit.
	if (!pool) return NULL;

	void *ptr = tlsf_realloc(pool, p, size);
	modm_assert_debug(p, "core", "heap", "realloc", size);
	return ptr;
}

void __wrap__free_r(struct _reent *r, void *p)
{
	(void) r;
	tlsf_t pool = get_tlsf_for_ptr(p);
	// if pointer belongs to no pool, exit.
	if (!pool) return;
	tlsf_free(pool, p);
}

// _sbrk_r is empty
void *
_sbrk_r(struct _reent *r,  ptrdiff_t size)
{
	(void) r;
	(void) size;
	return NULL;
}

}

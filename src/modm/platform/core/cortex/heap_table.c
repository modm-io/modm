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

/**
 * @ingroup 	{{partname}}
 * @defgroup	{{partname}}_core Core
 */

#include <stddef.h>
#include <stdint.h>

// ----------------------------------------------------------------------------
extern uint32_t __table_heap_start[];
extern uint32_t __table_heap_end[];

// Finds the largest heap with declared traits
void
modm_heap_table_find_largest(const uint32_t trait_mask, uint32_t **start, uint32_t **end)
{
	typedef struct
	{
		uint32_t traits;
		uint32_t * start;
		uint32_t * end;
	} __attribute__((packed)) table_pool_t;

	uint32_t current_size = 0;
	*start = NULL;
	*end = NULL;

	for (table_pool_t *table = (table_pool_t *)__table_heap_start;
		 table < (table_pool_t *)__table_heap_end;
		 table++)
	{
		// we only care for generic enough regions
		if (table->traits & trait_mask)
		{
			size_t size = (size_t) table->end - (size_t) table->start;

			// if the start address is equal to the end address
			// we can extend the previous memory region with this one
			if (table->start == *end) {
				*end = table->end;
				current_size += size;
			}
			else if (size >= current_size) {
				// otherwise we found a larger region
				*start = table->start;
				*end = table->end;
				current_size = size;
			}
		}
	}
}

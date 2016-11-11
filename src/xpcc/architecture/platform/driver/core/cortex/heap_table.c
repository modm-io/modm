// coding: utf-8
/* Copyright (c) 2013-2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
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
xpcc_heap_table_find_largest(const uint32_t trait_mask, uint32_t **start, uint32_t **end)
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

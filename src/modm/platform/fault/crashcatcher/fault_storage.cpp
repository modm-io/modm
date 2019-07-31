/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "fault_storage.hpp"
#include <modm/architecture/utils.hpp>
#include <modm/platform/core/hardware_init.hpp>

typedef struct
{
	uint32_t const traits;
	uint8_t *const start;
	uint8_t *const end;
} __attribute__((packed)) table_pool_t;

extern "C" const table_pool_t __table_heap_start[];
extern "C" const table_pool_t __table_heap_end[];

static constexpr uint32_t magic_start = 0xBAADC0DE;
static constexpr uint32_t magic_end = 0xC0FFEEEE;
static modm_section(".noinit") uint32_t marker_start;
static modm_section(".noinit") const uint8_t* marker_end_ptr;
static const uint8_t* report_end{0};
extern "C" uint8_t *g_crashCatcherStack;

namespace modm::platform
{

const uint32_t *fault_storage_heap_start{nullptr};

size_t
FaultStorage::openRead()
{
	report_end = __table_heap_start->start;
	// Check for start marker
	if (marker_start == magic_start and marker_end_ptr)
	{
		// Check if pointer is in any heap and sum up total size
		size_t size{0};
		for (auto t = __table_heap_start; t < __table_heap_end; t++)
		{
			if (t->start <= marker_end_ptr and marker_end_ptr < t->end)
			{
				if (*reinterpret_cast<const uint32_t*>(marker_end_ptr) == magic_end)
				{
					report_end = marker_end_ptr;
					size += marker_end_ptr - t->start;
					ptrdiff_t end = ptrdiff_t(marker_end_ptr + sizeof(magic_end));
					// Align end pointer to nearest 32-bits
					end = (end + 3) & ~3;
					fault_storage_heap_start = reinterpret_cast<const uint32_t*>(end);
					return size;
				}
				else break;
			}
			size += t->end - t->start;
		}
	}
	closeRead();
	return 0;
}

void
FaultStorage::closeRead()
{
	marker_start = 0;
	marker_end_ptr = nullptr;
}

void
FaultStorage::openWrite()
{
	current = begin();
	marker_start = magic_start;
	marker_end_ptr = nullptr;
}

bool
FaultStorage::write(const void* vdata, size_t element_size, size_t elements)
{
	const uint8_t *data = reinterpret_cast<const uint8_t *>(vdata);
	while(elements--)
	{
		uint32_t value;
		switch(element_size)
		{
			case 1:
				asm volatile("ldrb %0, [%1]" : "=r" (value) : "r" (data));
				break;
			case 2:
				asm volatile("ldrh %0, [%1]" : "=r" (value) : "r" (data));
				break;
			case 4:
				asm volatile("ldr %0, [%1]" : "=r" (value) : "r" (data));
				break;
			default:
				return false;
		}
		data += element_size;
		if (not write(reinterpret_cast<const uint8_t *>(&value), element_size))
			return false;
	}
	return true;
}

bool
FaultStorage::write(const uint8_t *data, size_t size)
{
	while(size--)
	{
		*const_cast<uint8_t*>(current.pos) = *data++;
		current++;
	}
	return true;
}

void
FaultStorage::closeWrite()
{
	marker_end_ptr = current.pos;
	write(&magic_end, sizeof(magic_end));
}

FaultStorage::Iterator
FaultStorage::begin()
{
	return Iterator((const void*)__table_heap_start, __table_heap_start->start);
}

FaultStorage::Iterator
FaultStorage::end()
{
	return Iterator((const void*)__table_heap_start, report_end);
}

// ========= Iterator =========

FaultStorage::Iterator::Iterator(const void *table, const uint8_t* pos):
	table(table), pos(pos)
{}

uint8_t
FaultStorage::Iterator::operator*() const
{
	return *pos;
}

FaultStorage::Iterator&
FaultStorage::Iterator::operator++()
{
	auto t = (const table_pool_t *)table;
	if (pos >= t->end or (pos + 1) == g_crashCatcherStack) {
		if (t < __table_heap_end) {
			t++;
			pos = t->start;
			table = (const void*)t;
		}
		return *this;
	}
	pos++;
	return *this;
}

FaultStorage::Iterator
FaultStorage::Iterator::operator++(int)
{
	Iterator it{table, pos};
	++*this;
	return it;
}

bool
FaultStorage::Iterator::operator==(const Iterator& other) const
{
	return pos == other.pos;
}

bool
FaultStorage::Iterator::operator!=(const Iterator& other) const
{
	return pos != other.pos;
}

}

MODM_HARDWARE_INIT_NAME(fault_open_read, modm::platform::FaultStorage::openRead);

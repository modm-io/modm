// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "ram.hpp"
#include <new>
#include <xpcc/architecture/interface/memory.hpp>
#include <xpcc/architecture/interface/assert.hpp>

void *
operator new(size_t size)
{
	void * ptr = xpcc::avr::allocateMemory(size);
	xpcc_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new[](size_t size)
{
	void * ptr = xpcc::avr::allocateMemory(size);
	xpcc_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new(size_t size, xpcc::MemoryTraits)
{
	void * ptr = xpcc::avr::allocateMemory(size);
	xpcc_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new[](size_t size, xpcc::MemoryTraits)
{
	void * ptr = xpcc::avr::allocateMemory(size);
	xpcc_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void
operator delete(void* ptr)
{
	xpcc::avr::freeMemory(ptr);
}

void
operator delete(void* ptr, size_t)
{
	xpcc::avr::freeMemory(ptr);
}

void
operator delete[](void* ptr)
{
	xpcc::avr::freeMemory(ptr);
}

void
operator delete[](void* ptr, size_t)
{
	xpcc::avr::freeMemory(ptr);
}

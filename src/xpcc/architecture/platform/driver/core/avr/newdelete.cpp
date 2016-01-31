// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "ram.hpp"

void *
operator new(size_t size)
{
	return xpcc::avr::allocateMemory(size);
}

void *
operator new[](size_t size)
{
	return xpcc::avr::allocateMemory(size);
}

void
operator delete(void* ptr)
{
	xpcc::avr::freeMemory(ptr);
}

void
operator delete(void* ptr, size_t size __attribute__((unused)))
{
	xpcc::avr::freeMemory(ptr);
}

void
operator delete[](void* ptr)
{
	xpcc::avr::freeMemory(ptr);
}

void
operator delete[](void* ptr, size_t size __attribute__((unused)))
{
	xpcc::avr::freeMemory(ptr);
}


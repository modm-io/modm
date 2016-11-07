/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2015, Christian Menard
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "ram.hpp"
#include <xpcc/architecture/interface/memory.hpp>

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

void *
operator new(size_t size, xpcc::MemoryTraits)
{
	return xpcc::avr::allocateMemory(size);
}

void *
operator new[](size_t size, xpcc::MemoryTraits)
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

/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2014, 2016-2017, Niklas Hauser
 * Copyright (c) 2015, Christian Menard
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <new>
#include <modm/architecture/interface/memory.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <modm/platform/core/ram.hpp>

void *
operator new(size_t size)
{
	void * ptr = modm::platform::allocateMemory(size);
	modm_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new[](size_t size)
{
	void * ptr = modm::platform::allocateMemory(size);
	modm_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new(size_t size, modm::MemoryTraits)
{
	void * ptr = modm::platform::allocateMemory(size);
	modm_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new[](size_t size, modm::MemoryTraits)
{
	void * ptr = modm::platform::allocateMemory(size);
	modm_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void*
operator new(std::size_t size, const std::nothrow_t&) noexcept
{
	return modm::platform::allocateMemory(size);
}

void*
operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
	return modm::platform::allocateMemory(size);
}

void
operator delete(void* ptr)
{
	modm::platform::freeMemory(ptr);
}

void
operator delete(void* ptr, size_t)
{
	modm::platform::freeMemory(ptr);
}

void
operator delete[](void* ptr)
{
	modm::platform::freeMemory(ptr);
}

void
operator delete[](void* ptr, size_t)
{
	modm::platform::freeMemory(ptr);
}

void
operator delete(void* ptr, const std::nothrow_t&) noexcept
{
	modm::platform::freeMemory(ptr);
}

void
operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
	modm::platform::freeMemory(ptr);
}

/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2012, 2014, 2016, 2020, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>                   // for prototypes of malloc() and free()
#include <new>
#include <modm/architecture/interface/memory.hpp>
#include <modm/architecture/interface/assert.hpp>

extern "C" void * malloc_tr(size_t, uint32_t);

// ----------------------------------------------------------------------------
void *
operator new(size_t size) throw ()
{
	void * ptr = malloc(size);
	modm_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new[](size_t size) throw ()
{
	void * ptr = malloc(size);
	modm_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new(size_t size, std::nothrow_t) noexcept
{
	return malloc(size);
}

void *
operator new[](size_t size, std::nothrow_t) noexcept
{
	return malloc(size);
}

void *
operator new(size_t size, modm::MemoryTraits traits) noexcept
{
	void * ptr = malloc_tr(size, traits.value);
	modm_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void *
operator new[](size_t size, modm::MemoryTraits traits) noexcept
{
	void * ptr = malloc_tr(size, traits.value);
	modm_assert(ptr, "core", "heap", "new", size);
	return ptr;
}

void
operator delete(void *ptr) noexcept
{
	free(ptr);
}

void
operator delete(void* ptr, size_t) noexcept
{
	free(ptr);
}

void
operator delete[](void* ptr) noexcept
{
	free(ptr);
}

void
operator delete[](void* ptr, size_t) noexcept
{
	free(ptr);
}

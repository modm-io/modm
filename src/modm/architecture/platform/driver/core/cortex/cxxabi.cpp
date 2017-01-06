/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2011-2012, 2014, 2016, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * \file	cxxabi.cpp
 * \brief	Minimal C++ support, no exception handling, no RTTI
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>                   // for prototypes of malloc() and free()
#include <modm/architecture/interface/memory.hpp>

extern "C"
{
	// ------------------------------------------------------------------------
	void* __dso_handle = (void *) &__dso_handle;
	
	void
	__cxa_pure_virtual()
	{
		// put error handling here
	}
	
	// ------------------------------------------------------------------------
	__extension__ typedef int __guard __attribute__((mode (__DI__)));
	
	int
	__cxa_guard_acquire(__guard *g)
	{
		return !*(char *)(g);
	}
	
	void
	__cxa_guard_release (__guard *g)
	{
		*(char *) g = 1;
	}
	
	void
	__cxa_guard_abort (__guard *)
	{
	}
	
	// ------------------------------------------------------------------------
	int
	__aeabi_atexit(void */*object*/, void (*/*destructor*/)(void *), void */*dso_handle*/)
	{
		return 0;
	}

	extern void * malloc_tr(size_t, uint32_t);
}

// ----------------------------------------------------------------------------
void *
operator new(size_t size) throw ()
{
	return malloc(size);
}

void *
operator new[](size_t size) throw ()
{
	return malloc(size);
}

void *
operator new(size_t size, modm::MemoryTraits traits)
{
	return malloc_tr(size, traits.value);
}

void *
operator new[](size_t size, modm::MemoryTraits traits)
{
	return malloc_tr(size, traits.value);
}

void
operator delete(void *p) throw ()
{
	free(p);
}

void
operator delete[](void* p) throw ()
{
	free(p);
}

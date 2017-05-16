/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/interface/assert.hpp>

extern "C"
{
	/**
	 * \brief Pure-virtual workaround.
	 *
	 * The avr-libc does not support a default implementation for handling
	 * possible pure-virtual calls. This is a short and empty workaround for this.
	 */
	void
	__cxa_pure_virtual()
	{
		modm_assert_debug(0, "core", "cxa", "purevirtual");
	}

	__extension__ typedef int __guard __attribute__((mode (__DI__)));

	int
	__cxa_guard_acquire(__guard *g)
	{
		return !*(char *)(g);
	}

	void
	__cxa_guard_release (__guard *g)
	{
		*(char *)g = 1;
	}

	void
	__cxa_guard_abort (__guard *)
	{
	}

	int
	__cxa_atexit (void (* /*destructor*/)(void *), void */*object*/, void */*dso_handle*/)
	{
		return 0;
	}

	void* __dso_handle = (void*) &__dso_handle;
}

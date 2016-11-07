/*
 * Copyright (c) 2013-2014, 2016, Niklas Hauser
 * Copyright (c) 2013-2014, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/detect.hpp>

#ifdef XPCC__OS_HOSTED

#include <xpcc/architecture/interface/memory.hpp>
#include <stdlib.h>

void *
operator new(size_t size, xpcc::MemoryTraits)
{
	return ::operator new(size);
}

void *
operator new[](size_t size, xpcc::MemoryTraits)
{
	return ::operator new[](size);
}

#endif

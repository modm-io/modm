// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
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

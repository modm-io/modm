// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AVR_RAM_HPP
#define	XPCC_AVR_RAM_HPP

#include <stdint.h>
#include <cstddef>

namespace xpcc
{

namespace avr
{

void *
allocateMemory(std::size_t requestedSize);

void
freeMemory(void *ptr);

// TODO functions to retrieve status informations about used memory

}	// namespace avr

}	// namespace xpcc

#endif	// XPCC_AVR_RAM_HPP


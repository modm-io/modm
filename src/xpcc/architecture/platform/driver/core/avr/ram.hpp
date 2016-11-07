/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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


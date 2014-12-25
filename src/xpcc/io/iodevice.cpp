// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "iodevice.hpp"

// ----------------------------------------------------------------------------
void
xpcc::IODevice::write(const char* str)
{
	char c;
	while ((c = *str++)) {
		this->write(c);
	}
}

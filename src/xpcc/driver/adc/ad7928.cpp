// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2017, Christopher Durand
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "ad7928.hpp"

// ----------------------------------------------------------------------------
xpcc::IOStream&
xpcc::operator<<(xpcc::IOStream& out, const xpcc::ad7928::Data& data)
{
	out	<< "(channel = " << static_cast<uint16_t>(data.channel())
		<< ", value = " << data.value() << ")";

	return out;
}

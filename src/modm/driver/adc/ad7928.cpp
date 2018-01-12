// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2017, Christopher Durand
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "ad7928.hpp"

// ----------------------------------------------------------------------------
modm::IOStream&
modm::operator<<(modm::IOStream& out, const modm::ad7928::Data& data)
{
	out	<< "(channel = " << static_cast<uint16_t>(data.channel())
		<< ", value = " << data.value() << ")";

	return out;
}

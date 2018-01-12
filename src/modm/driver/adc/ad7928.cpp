// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2017, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

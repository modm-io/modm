/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "ad7280a.hpp"

// ----------------------------------------------------------------------------
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& s, const ad7280a::RegisterValue& c)
{
	s	<< " (dev=" << c.device
		<< ", reg=" << c.registerAddress
		<< ", val=" << c.value
		<< ", ack=" << c.acknowledge << ")";
	
	return s;
}

// ----------------------------------------------------------------------------
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& s, const ad7280a::ConversionValue& c)
{
	s	<< " (dev=" << c.device
		<< ", ch =" << c.channel
		<< ", val=" << c.value
		<< ", ack=" << c.acknowledge << ")";
	
	return s;
}

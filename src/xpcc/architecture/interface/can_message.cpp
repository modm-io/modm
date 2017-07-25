// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <stdint.h>
#include <inttypes.h>

#include <xpcc/io/iostream.hpp>
#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/interface/can_message.hpp>

namespace xpcc
{

namespace can
{

xpcc::IOStream&
operator << (xpcc::IOStream& s, const xpcc::can::Message m)
{
    s.printf("id = %04" PRIx32 ", len = ", m.identifier);
    s << m.length;
    s.printf(", flags = %c%c, data = ",
             m.flags.rtr ? 'R' : 'r',
             m.flags.extended ? 'E' : 'e');
    for (uint_fast8_t ii = 0; ii < m.length; ++ii) {
        s.printf("%02x ", m.data[ii]);
    }
    return s;
}

bool
xpcc::can::Message::operator == (const xpcc::can::Message& rhs) const
{
	if ((this->identifier     == rhs.identifier) and
		(this->length         == rhs.length)     and
		(this->flags.rtr      == rhs.flags.rtr)  and
		(this->flags.extended == rhs.flags.extended))
	{
		bool eq = true;
		for (uint8_t ii = 0; ii < this->length; ++ii)
		{
			if (this->data[ii] != rhs.data[ii]) {
				eq = false;
			}
		}
		return eq;
	}
	return false;
}
} // can namespace
} // xpcc namespace

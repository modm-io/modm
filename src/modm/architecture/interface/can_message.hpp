/*
 * Copyright (c) 2014, 2016, Niklas Hauser
 * Copyright (c) 2015-2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_CAN_MESSAGE_HPP
#define MODM_CAN_MESSAGE_HPP

#include <stdint.h>
#include <string.h>  // strlen
#include <algorithm>
#include <modm/architecture/utils.hpp>

namespace modm::can
{

/// Representation of a CAN message
/// @ingroup modm_architecture_can
struct Message
{
	inline Message(uint32_t inIdentifier = 0, uint8_t inLength = 0) :
		identifier(inIdentifier), flags(), length(inLength)
	{
	}

	// Create CAN message from long data in Network Order.
	inline Message(uint32_t inIdentifier, uint8_t inLength, const uint64_t &inData, bool extended=false) :
		identifier(inIdentifier), length(std::min(inLength, uint8_t(8)))
	{
		flags.extended = extended;
		const uint8_t *inDataB = reinterpret_cast<const uint8_t *>(&inData);
		for (uint8_t ii = 0; ii < length; ++ii)
 			data[ii] = inDataB[length - ii - 1];
	}

	inline uint32_t
	getIdentifier() const
	{
		return identifier;
	}

	inline void
	setIdentifier(uint32_t id)
	{
		identifier = id;
	}

	inline void
	setExtended(bool extended = true)
	{
		flags.extended = (extended) ? 1 : 0;
	}

	inline bool
	isExtended() const
	{
		return (flags.extended != 0);
	}

	inline void
	setRemoteTransmitRequest(bool rtr = true)
	{
		flags.rtr = (rtr) ? 1 : 0;
	}

	inline bool
	isRemoteTransmitRequest() const
	{
		return (flags.rtr != 0);
	}

	inline uint8_t
	getLength() const
	{
		return length;
	}

	inline void
	setLength(uint8_t len)
	{
		length = len;
	}

public:
	uint32_t identifier;
	uint8_t modm_aligned(4) data[8];
	struct Flags
	{
		Flags() :
			rtr(0), extended(1)
		{
		}

		bool rtr : 1;
		bool extended : 1;
	} flags;
	uint8_t length;

public:
	inline bool
	operator == (const modm::can::Message& rhs) const
	{
		return ((this->identifier     == rhs.identifier) and
				(this->length         == rhs.length)     and
				(this->flags.rtr      == rhs.flags.rtr)  and
				(this->flags.extended == rhs.flags.extended) and
				std::equal(data, data + length, rhs.data));
	}

	inline bool
	operator < (const modm::can::Message& rhs) const
	{
		return (this->identifier << (this->flags.extended ? 0 : 18))
			< (rhs.identifier << (rhs.flags.extended ? 0 : 18));
	}
};

}	// namespace modm::can

#if MODM_HAS_IOSTREAM
#include <inttypes.h>
#include <modm/io/iostream.hpp>

namespace modm
{

inline modm::IOStream&
operator << (modm::IOStream& s, const modm::can::Message& m)
{
	s.printf("id = %04" PRIx32 ", len = ", m.identifier);
	s << m.length;
	s.printf(", flags = %c%c, data = ",
			 m.flags.rtr ? 'R' : 'r',
			 m.flags.extended ? 'E' : 'e');
	if (not m.isRemoteTransmitRequest()) {
		for (uint_fast8_t ii = 0; ii < m.length; ++ii) {
			s.printf("%02x ", m.data[ii]);
		}
	}
	return s;
}

} // modm namespace
#endif

#endif // MODM_CAN_MESSAGE_HPP

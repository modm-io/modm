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

#ifndef XPCC_CAN__MESSAGE_HPP
#define XPCC_CAN__MESSAGE_HPP

#include <stdint.h>
#include <modm/io/iostream.hpp>
#include <modm/architecture/utils.hpp>

namespace xpcc
{

namespace can
{

/// Representation of a CAN message
/// @ingroup	can
struct Message
{
	Message(const uint32_t& inIdentifier = 0, uint8_t inLength = 0) :
		identifier(inIdentifier), flags(), length(inLength)
	{
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
	uint8_t xpcc_aligned(4) data[8];
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
	bool
	operator == (const xpcc::can::Message& rhs) const;
};

xpcc::IOStream&
operator << (xpcc::IOStream& s, const xpcc::can::Message m);

}	// namespace can

}	// namespace xpcc

#endif // XPCC_CAN__MESSAGE_HPP

// ----------------------------------------------------------------------------
// coding: utf-8
/* Copyright (c) 2009, 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_CAN__MESSAGE_HPP
#define XPCC_CAN__MESSAGE_HPP

#include <stdint.h>
#include <xpcc/io/iostream.hpp>
#include <xpcc/architecture/utils.hpp>

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

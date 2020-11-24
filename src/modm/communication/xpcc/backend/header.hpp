/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2013, 2018, Niklas Hauser
 * Copyright (c) 2013, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_HEADER_HPP
#define	XPCC_HEADER_HPP

#include <stdint.h>

#include <modm/container/smart_pointer.hpp>

namespace xpcc
{
	/**
	 * \brief 	Header of a packet
	 *
	 * \ingroup	modm_communication_xpcc_backend
	 * \author	Martin Rosekeit
	 * \author	Fabian Greif
	 */
	struct Header
	{
	public:
		enum class Type : uint8_t
		{
			REQUEST,
			RESPONSE,
			NEGATIVE_RESPONSE,
			TIMEOUT,
		};

	public:
		Header() :
			type(Type::REQUEST),
			isAcknowledge(false),
			destination(0),
			source(0),
			packetIdentifier(0)
		{
		}

		Header(Type type, bool ack,
				uint8_t dest, uint8_t src,
				uint8_t id) :
			type(type),
			isAcknowledge(ack),
			destination(dest),
			source(src),
			packetIdentifier(id)
		{
		}

		Type type;
		bool isAcknowledge;

		uint8_t destination;
		uint8_t source;
		uint8_t packetIdentifier;

		bool
		operator == (const Header& other) const;
	};

} // namespace xpcc

namespace modm
{

/**
 * \brief	Stream operator for a packet header
 * \ingroup	backend
 */
modm::IOStream&
operator << (modm::IOStream& s, const xpcc::Header& header);

} // modm namespace

#endif	// XPCC_HEADER_HPP

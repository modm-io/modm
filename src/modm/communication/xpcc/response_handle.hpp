/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, 2011-2012, Georgi Grinshpun
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_RESPONSE_HANDLE_HPP
#define	XPCC_RESPONSE_HANDLE_HPP

#include "backend/header.hpp"

namespace xpcc
{
	/**
	 * \brief	Response to an action call
	 *
	 * \ingroup	modm_communication_xpcc
	 */
	class ResponseHandle
	{
		friend class Communicator;

	public:
		ResponseHandle() :
			destination(0)
		{
		}

		explicit ResponseHandle(const Header& header) :
			destination(header.source),
			packetIdentifier(header.packetIdentifier)
		{
		}

	public:
		inline uint8_t
		getDestination() const
		{
			return this->destination;
		}

		inline uint8_t
		getIdentifier() const
		{
			return this->packetIdentifier;
		}

	protected:
		uint8_t destination;
		uint8_t packetIdentifier;
	};
}

#endif // XPCC_RESPONSE_HANDLE_HPP

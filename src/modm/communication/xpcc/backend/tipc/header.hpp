/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, 2014, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TIPC_HEADER_HPP_
#define XPCC_TIPC_HEADER_HPP_

#include <cstring>

namespace xpcc
{
	namespace tipc
	{
		/**
		 * the first 64 ids (types) are used by tipc itself
		 * \ingroup tipc
		 */
		static const unsigned int TYPE_ID_OFFSET = 64;
		
		/// \ingroup tipc
		static const unsigned int EVENT_OFFSET   = 0x10000;
		
		/// \ingroup tipc
		static const unsigned int REQUEST_OFFSET = 0x20000;
		
		/**
		 * \brief		The header, that are send over the tipc.
		 *
		 * \ingroup		tipc
		 */
		struct Header
		{
			enum {
				DOMAIN_ID_UNDEFINED = 0, // no domain id set => listen to all messanges on the tipc
			};

			inline
			Header() :
				size( 0 ),
				domainId( DOMAIN_ID_UNDEFINED )
			{}

			// \param size of the payload in byte
			// \param id of the domain in that the message is transmitted
			inline
			Header( size_t size, unsigned int domainId = DOMAIN_ID_UNDEFINED) :
				size( size ),
				domainId( domainId )
			{}

			size_t size;
			unsigned int domainId;
		};
	};
};

#endif /* XPCC_TIPC_HEADER_HPP_ */

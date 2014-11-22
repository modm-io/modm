// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

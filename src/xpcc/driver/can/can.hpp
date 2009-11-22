// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__CAN_HPP
#define XPCC__CAN_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * @ingroup	driver
	 * @headerfile	<xpcc/driver/can/can.hpp>
	 */
	class Can
	{
	public:
		/// @brief	supported bitrates
		typedef enum {
			BITRATE_10_KBPS	= 0,
			BITRATE_20_KBPS	= 1,
			BITRATE_50_KBPS	= 2,
			BITRATE_100_KBPS = 3,
			BITRATE_125_KBPS = 4,
			BITRATE_250_KBPS = 5,
			BITRATE_500_KBPS = 6,
			BITRATE_1_MBPS = 7,
		} Bitrate;
		
		/// @brief	
		typedef enum {
			NORMAL,
			LISTEN_ONLY,
			LOOPBACK,
		} Mode;
		
		/// @brief	Representation of a CAN message
		struct Message
		{
			Message(const uint32_t& identifier = 0, uint8_t length = 0) :
				identifier(identifier), data(), flags(), length(length)
			{
			}
			
			uint32_t identifier;
			uint8_t data[8];
			struct {
				bool rtr : 1;
				bool extended : 1;
			} flags;
			uint8_t length;
		};
	
	public:
		
	};
}

#endif // XPCC__CAN_HPP

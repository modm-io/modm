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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_CAN__MESSAGE_HPP
#define XPCC_CAN__MESSAGE_HPP

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>

namespace xpcc
{
	/**
	 * \ingroup	can
	 */
	namespace can
	{
		/**
		 * \brief	Supported CAN bitrates
		 * \ingroup	can
		 */
		enum Bitrate
		{
			BITRATE_10_KBPS	= 0,
			BITRATE_20_KBPS	= 1,
			BITRATE_50_KBPS	= 2,
			BITRATE_100_KBPS = 3,
			BITRATE_125_KBPS = 4,
			BITRATE_250_KBPS = 5,
			BITRATE_500_KBPS = 6,
			BITRATE_1_MBPS = 7,
		};
		
		/**
		 * \brief	Possible modes of the CAN controller
		 * 
		 * NORMAL is the standard operation mode, in LISTEN_ONLY mode it is
		 * only possible to receive messages and in LOOPBACK mode all messages
		 * send are not send to the CAN bus but are received immediately by
		 * the own controller. 
		 */
		enum Mode
		{
			NORMAL,
			LISTEN_ONLY,
			LOOPBACK,
		};
		
		enum BusState
		{
			CONNECTED,

			// This bit is set by hardware when the warning limit has been
			// reached (Receive Error Counter or Transmit Error Counter ≥ 96).
			ERROR_WARNING,

			// This bit is set by hardware when the Error Passive limit has
			// been reached (Receive Error Counter or Transmit Error Counter > 127).
			ERROR_PASSIVE,

			// This bit is set by hardware when it enters the bus-off state.
			// The bus-off state is entered on TEC overflow, greater than 255
			BUS_OFF,
		};

		/**
		 * \brief	Representation of a CAN message
		 * \ingroup	can
		 */
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
			
		public:
			uint32_t identifier;
			uint8_t ATTRIBUTE_ALIGNED(4) data[8];
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
		};
	}
}

#endif // XPCC_CAN__MESSAGE_HPP

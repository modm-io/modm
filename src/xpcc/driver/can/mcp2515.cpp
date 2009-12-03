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

#include "mcp2515.hpp"

namespace xpcc
{
	namespace mcp2515
	{
		FLASH(uint8_t configuration[24]) =
		{
#if MCP2515_CLOCK == 16000000

			// 10 kbps
			0x04,		// CNF3
			0xb6,		// CNF2
			0xe7,		// CNF1
			
			// 20 kbps
			0x04,
			0xb6,
			0xd3,
		
			// 50 kbps
			0x04,
			0xb6,
			0xc7,
			
			// 100 kbps
			0x04,
			0xb6,
			0xc3,
			
			// 125 kbps
			(1 << PHSEG21),
			(1 << BTLMODE) | (1 << PHSEG11),
			(1 << BRP2) | (1 << BRP1) | (1 << BRP0),
			
			// 250 kbps
			0x03,
			0xac,
			0x81,
			
			// 500 kbps
			0x03,
			0xac,
			0x80,
			
			// 1 Mbps
			(1 << PHSEG21),
			(1 << BTLMODE) | (1 << PHSEG11),
			0,

#elif MCP2515_CLOCK == 20000000

			// 10 kbps
			0x04,		// CNF3
			0xb6,		// CNF2
			0x31,		// CNF1
			
			// 20 kbps
			0x04,
			0xb6,
			0x18,
		
			// 50 kbps
			0x04,
			0xb6,
			0x18,
			
			// 100 kbps
			0x04,
			0xb6,
			0x04,
			
			// 125 kbps
			0x04,
			0xb6,
			0x03,
			
			// 250 kbps
			0x04,
			0xb6,
			0x01,
			
			// 500 kbps
			0x04,
			0xb6,
			0x00,
			
			// 1 Mbps
			0x02,
			0x92,
			0x00,

#else
	#error "Unknown value for MCP2515 clock frequency, please add the defines in 'mcp2515.cpp'"
#endif
		};
	}
}

/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "mcp2515.hpp"
#include "xpcc_config.hpp"

namespace xpcc
{
	namespace mcp2515
	{
		FLASH_STORAGE(uint8_t configuration[24]) =
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
			PHSEG21,
			BTLMODE | PHSEG11,
			MCP2515_BRP2 | MCP2515_BRP1 | MCP2515_BRP0,
			
			// 250 kbps
			0x03,
			0xac,
			0x81,
			
			// 500 kbps
			0x03,
			0xac,
			0x80,
			
			// 1 Mbps
			PHSEG21,
			BTLMODE | PHSEG11,
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
#	error "Unknown value for MCP2515 clock frequency, please add the defines in 'mcp2515.cpp'"
#endif
		};
	}
}

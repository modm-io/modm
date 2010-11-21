// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2010, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Roboterclub Aachen e.V. nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

#include <util/atomic.h>

#include "at90can.h"

// ----------------------------------------------------------------------------
void
at90can_send_message(command_t type, uint8_t length)
{
	while (true)
	{
		// check if there is any free MOb
		if (at90can_free_buffer == 0) {
			continue;
		}
		
		for (uint8_t mob = 8; mob < 15; mob++)
		{
			// load MOb page
			CANPAGE = mob << 4;
			
			// check if the MOb is in use
			if ((CANCDMOB & ((1 << CONMOB1) | (1 << CONMOB0))) == 0)
			{
				// clear flags (read-write-cycle required)
				CANSTMOB &= 0;
				
				// set identifier
				CANIDT4 = 0;
				CANIDT3 = 0;
				CANIDT2 = (uint8_t) (0x7fe << 5);
				CANIDT1 = (uint8_t) (0x7fe >> 3);
				
				CANMSG = BOOTLOADER_BOARD_ID;
				CANMSG = type;
				CANMSG = message_number;
				CANMSG = message_data_counter;
				
				// copy data
				const uint8_t *p = message_data;
				for (uint8_t i = 0; i < length; i++) {
					CANMSG = *p++;
				}
				
				// enable MOb interrupt
				CANIE1 |= (1 << (mob - 8));
				
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					at90can_free_buffer--;
				}
				
				// enable transmission
				CANCDMOB = (1 << CONMOB0) | (length + 4);
				
				return;
			}
		}
	}
}

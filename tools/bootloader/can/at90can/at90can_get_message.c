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
static bool
check_message(void)
{
	if (at90can_messages_waiting > 0)
		return true;
	else
		return false;
}

// ----------------------------------------------------------------------------
command_t
at90can_get_message(void)
{
	// check if there is any waiting message
	if (!check_message())
		return NO_MESSAGE;
	
	// find the MOb with the received message
	for (uint8_t mob = 0; mob < 8; mob++)
	{
		CANPAGE = mob << 4;
		
		if (CANSTMOB & (1 << RXOK))
		{
			// clear flags (read-write-cycle required)
			CANSTMOB &= 0;
			
			// read status
			message_data_length = CANCDMOB & 0x0f;
			
			uint8_t type = NO_MESSAGE;
			uint8_t board_id = CANMSG;
			if (message_data_length >= 4 && board_id == BOOTLOADER_BOARD_ID)
			{
				type = CANMSG;
				message_number = CANMSG;
				message_data_counter = CANMSG;
				message_data_length -= 4;
				
				// read data
				for (uint8_t i = 0; i < message_data_length; i++) {
					message_data[i] = CANMSG;
				}
			}
			
			// mark message as processed
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				at90can_messages_waiting--;
			}
			
			// re-enable interrupts
			CANIE2 |= (1 << mob);
			
			// clear flags
			CANCDMOB = (1 << CONMOB1);
			
			return type;
		}
	}
	
	return NO_MESSAGE;		// should never happen
}

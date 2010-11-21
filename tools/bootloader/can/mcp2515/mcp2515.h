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

#ifndef	MCP2515_H
#define	MCP2515_H

#include "defaults.h"
#include "mcp2515_defs.h"

#include <inttypes.h>

extern uint8_t message_number;			//!< Running number of the messages
extern uint8_t message_data_counter;	
extern uint8_t message_data_length;		//!< Length of the data-field
extern uint8_t message_data[4];

typedef enum {
	// every bootloader type has this commands
	IDENTIFY		= 1,
	SET_ADDRESS		= 2,
	DATA			= 3,
	START_APP		= 4,
	
	// only avilable in the "bigger" versions
	GET_FUSEBITS	= 5,
	CHIP_ERASE		= 6,
	
	REQUEST					= 0x00,
	SUCCESSFULL_RESPONSE	= 0x40,
	ERROR_RESPONSE			= 0x80,
	WRONG_NUMBER_REPSONSE	= 0xC0,
	
	NO_MESSAGE		= 0x3f
} command_t;

#define	COMMAND_MASK			0x3F
#define	START_OF_MESSAGE_MASK	0x80

// ----------------------------------------------------------------------------
void
mcp2515_send_message(uint8_t type, uint8_t length);

// ----------------------------------------------------------------------------
uint8_t
mcp2515_get_message(void);

#endif	// MCP2515_H

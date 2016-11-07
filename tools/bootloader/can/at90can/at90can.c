/*
 * Copyright (c) 2010, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

#include "at90can.h"
#include "defaults.h"

// ----------------------------------------------------------------------------
volatile uint8_t at90can_messages_waiting;
volatile uint8_t at90can_free_buffer;			//!< Stores the number of currently free MObs

uint8_t message_number;
uint8_t message_data_counter;	
uint8_t message_data_length;
uint8_t message_data[4];

// ----------------------------------------------------------------------------
void
at90can_init(void)
{
	// switch CAN controller to reset mode
	CANGCON |= (1 << SWRES);
	
	// set CAN Bit Timing
	// (see datasheet page 260)

#if CAN_BITRATE == 10
	// 10 kbps
	CANBT1 = 0x7E;
	CANBT2 = 0x6E;
	CANBT3 = 0x7F;
#elif CAN_BITRATE == 20
	// 20 kbps
	CANBT1 = 0x62;
	CANBT2 = 0x0C;
	CANBT3 = 0x37;
#elif CAN_BITRATE == 50
	// 50 kbps
	CANBT1 = 0x26;
	CANBT2 = 0x0C;
	CANBT3 = 0x37;
#elif CAN_BITRATE == 100
	// 100 kbps
	CANBT1 = 0x12;
	CANBT2 = 0x0C;
	CANBT3 = 0x37;
#elif CAN_BITRATE == 125
	// 125 kbps
	CANBT1 = 0x0E;
	CANBT2 = 0x0C;
	CANBT3 = 0x37;
#elif CAN_BITRATE == 250
	// 250 kbps
	CANBT1 = 0x06;
	CANBT2 = 0x0C;
	CANBT3 = 0x37;
#elif CAN_BITRATE == 500
	// 500 kbps
	CANBT1 = 0x02;
	CANBT2 = 0x0C;
	CANBT3 = 0x37;
#elif CAN_BITRATE == 1000
	// 1 Mbps
	CANBT1 = 0x00;
	CANBT2 = 0x0C;
	CANBT3 = 0x36;
#endif
	
	// activate CAN transmit- and receive-interrupt
	CANGIT = 0;
	CANGIE = (1 << ENIT) | (1 << ENRX) | (1 << ENTX);
	
	// set timer prescaler to 199 which results in a timer
	// frequency of 10 kHz (at 16 MHz)
	CANTCON = 199;
	
	// disable interrupts
	CANIE1 = 0;
	CANIE2 = 0;
	
	// disable all MObs
	for (uint8_t mob = 0; mob < 15; mob++)
	{
		CANPAGE = (mob << 4);
		
		// disable MOb (read-write required)
		CANCDMOB &= 0;
		CANSTMOB &= 0;
	}
	
	// mark all MObs as free
	at90can_messages_waiting = 0;
	at90can_free_buffer = 7;
	
	// set filter for MOb 0 to 8
	for (uint8_t mob = 0; mob < 8; mob++)
	{
		CANPAGE = (mob << 4);
		
		CANSTMOB = 0;
		CANCDMOB = (1 << CONMOB1);
		
		// only standard, non-rtr frames with identifier 0x7ff
		CANIDT4 = 0;
		CANIDT3 = 0;
		CANIDT2 = (uint8_t) (0x7ff << 5);
		CANIDT1 = (uint8_t) (0x7ff >> 3);
		
		CANIDM4 = (1 << IDEMSK) | (1 << RTRMSK);
		CANIDM3 = 0;
		CANIDM2 = (uint8_t) (0x7ff << 5);
		CANIDM1 = (uint8_t) (0x7ff >> 3);
	}
	
	// enable interrupts for the MObs
	CANIE2 = 0xff;
	
	// activate CAN controller
	CANGCON = (1 << ENASTB);
}

// ----------------------------------------------------------------------------
// The CANPAGE register have to be restored after usage, otherwise it
// could cause trouble in the application programm.

ISR(CANIT_vect)
{
	uint8_t canpage;
	uint8_t mob;
	
	if ((CANHPMOB & 0xF0) != 0xF0)
	{
		// save MOb page register
		canpage = CANPAGE;
		
		// select MOb page with the highest priority
		CANPAGE = CANHPMOB & 0xF0;
		mob = (CANHPMOB >> 4);
		
		// a interrupt is only generated if a message was transmitted or received
		if (CANSTMOB & (1 << TXOK))
		{
			// clear MOb
			CANSTMOB &= 0;
			CANCDMOB &= 0;
			
			at90can_free_buffer++;
		}
		else {
			// a message was received successfully
			at90can_messages_waiting++;
		}
		
		// reset interrupt
		if (mob < 8) {
			CANIE2 &= ~(1 << mob);
		}
		else {
			CANIE1 &= ~(1 << (mob - 8));
		}
		
		// restore MOb page register
		CANPAGE = canpage;
	}
	else
	{
		// no MOb matches with the interrupt => general interrupt
		CANGIT |= 0;
	}
}

// ----------------------------------------------------------------------------
// Overflow of CAN timer
ISR(OVRIT_vect)
{
}

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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include <cstddef>

#include "twi_master.hpp"	

// ----------------------------------------------------------------------------
void
xpcc::SynchronousTwiMaster::initialize(uint8_t twbr, uint8_t twps)
{
	// Set bit rate register
	TWBR = twbr;
	TWSR = twps & 0x03;
	
	// Default content = SDA released.
	TWDR = 0xFF;
	TWCR = (1<<TWEN)|							// Enable TWI-interface and release TWI pins.
			(0<<TWIE)|(0<<TWINT)|				// Disable Interupt.
			(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|	// No Signal requests.
			(0<<TWWC);
}

// ----------------------------------------------------------------------------
bool
xpcc::SynchronousTwiMaster::start(uint8_t address)
{
	uint8_t twst;
	
	// Send START condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	while (!(TWCR & (1<<TWINT))) {
		// Wait until transmission completed
	}
	
	// Check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS;
	if ((twst != TW_START) && (twst != TW_REP_START)) {
		return false;
	}
	
	//Send device address
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	while (!(TWCR & (1 << TWINT))) {
		// Wait until transmission completed and ACK/NACK has been received
	}
	
	// Check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS;
	if ((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK)) {
		return false;
	}
	
	return true;
}

// ----------------------------------------------------------------------------
void
xpcc::SynchronousTwiMaster::stop()
{
	// Send stop condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	
	while (TWCR & (1 << TWINT)) {
		// Wait until stop condition is executed and bus released
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::SynchronousTwiMaster::write(uint8_t byte)
{
	// Send data to the previously addressed device
	TWDR = byte;
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	while (!(TWCR & (1 << TWINT))) {
		// Wait until transmission completed
	}
	
	// Check value of TWI Status Register. Mask prescaler bits
	if (TW_STATUS != TW_MT_DATA_ACK) {
		return false;
	}
	
	return true;
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::SynchronousTwiMaster::read(bool ack)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | ((ack) ? (1 << TWEA) : 0);
	
	while (!(TWCR & (1 << TWINT))) {
		// Wait until reception completed
	}
	
	return TWDR;
}

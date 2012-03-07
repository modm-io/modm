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
 * $Id: twi_master_asynchronous.cpp 734 2012-02-16 22:39:11Z salkinium $
 */
// ----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "twi_master.hpp"	

// TWI State machine value when finished
#define TWI_STATUS_DONE 0xff

// ----------------------------------------------------------------------------
// parameter advice
static volatile bool reading;
static volatile bool restartAfterReading;
static uint8_t twiAddress;

// buffer management
static const uint8_t *twiWriteBuffer;
static uint8_t *twiReadBuffer;
static uint8_t twiReadCounter;
static uint8_t twiWriteCounter;

// state
static volatile xpcc::i2c::BusState busState = xpcc::i2c::BUS_RESET;
static volatile bool occupied = false;
static volatile bool startConditionGenerated =  false;



// ----------------------------------------------------------------------------
/// TWI state machine interrupt handler
ISR(TWI_vect)
{
	switch(TW_STATUS)
	{
	case TW_START:
		// START has been transmitted
		// Fall through...
	case TW_REP_START:
		// REPEATED START has been transmitted
		
		if (!startConditionGenerated){
			// Disable interrupts
			TWCR = (1 << TWEN);
			startConditionGenerated = true;
			// busstate is set where startcondition is initiated
		}
		else{
			startConditionGenerated = false;
			// Load data register with TWI slave address
			TWDR = twiAddress | (reading ? xpcc::i2c::READ : xpcc::i2c::WRITE);
			// clear interrupt flag to send address
			TWCR |= (1 << TWINT);
		}
		break;
	
	case TW_MT_SLA_ACK:
		// SLA+W has been transmitted and ACK received
		// Fall through...
	case TW_MT_DATA_ACK:
		// Data byte has been transmitted and ACK received
		if (twiWriteCounter > 0)
		{
			// Decrement counter
			twiWriteCounter--;
			// Load data register with next byte
			TWDR = *twiWriteBuffer++;
			// TWI Interrupt enabled and clear flag to send next byte
			TWCR |= (1 << TWINT);
		}
		else
		{
			// Transfer finished
			// Disable interrupts
			TWCR = 1 << TWEN;
			busState = xpcc::i2c::BUS_WRITE;

			// Initiate STOP condition after last byte; TWI Interrupt disabled
//			TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);


		}
		break;
	
	case TW_MR_DATA_ACK:
		// Data byte has been received and ACK transmitted
		*twiReadBuffer++ = TWDR;
		twiReadCounter--;
		
		// Fall through...
	case TW_MR_SLA_ACK:
		// SLA+R has been transmitted and ACK received
		// See if last expected byte will be received ...
		if (twiReadCounter > 1) {
			// Send ACK after reception
			TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		}
		else {
			// Send NACK after next reception
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
		}
		break;
	
	case TW_MR_DATA_NACK:
		// Data byte has been received and NACK transmitted
		// => Transfer finished
		*twiReadBuffer++ = TWDR;
		twiReadCounter--;
		
		if (restartAfterReading){
			// Initiate START (restart) condition; TWI Interrupt stay enabled
			TWCR |= (1 << TWINT) | (1 << TWSTA);
			busState = xpcc::i2c::BUS_HOLD;
		}
		else{
			// Initiate STOP condition after last byte; TWI Interrupt disabled
			TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
			busState = xpcc::i2c::BUS_STOPPED;
		}
		break;
	
	case TW_MT_SLA_NACK:	//SLA+W transmitted, NACK received
	case TW_MT_DATA_NACK:	//data transmitted, NACK received
	case TW_MR_SLA_NACK:		//SLA+R transmitted, NACK received
		// generate a stop condition Disable interrupts
		TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
		busState = xpcc::i2c::BUS_RESET;
		startConditionGenerated = false;
		break;


	default:
		// Another Error condition
		// Reset TWI Interface; disable interrupt
		busState = xpcc::i2c::BUS_RESET;
		startConditionGenerated = false;

		TWCR = 0;
	}
}

// ----------------------------------------------------------------------------
void
xpcc::atmega::AsynchronousTwiMaster::initialize(uint8_t twbr, uint8_t twps)
{
	twiReadCounter = 0;
	twiWriteCounter = 0;
	
	// Initialize TWI clock
	TWBR = twbr;
	TWSR = twps & 0x03;
	
	// Load data register with default content; release SDA
	TWDR = 0xff;
	
	// Enable TWI peripheral with interrupt disabled
	TWCR = (1 << TWEN);
}


bool
xpcc::atmega::AsynchronousTwiMaster::start(uint8_t slaveAddress)
{
	if (getBusyState() == xpcc::i2c::FREE){
		// Copy address; clear R/~W bit in SLA+R/W address field
		twiAddress = slaveAddress & 0xfe;
		busState = xpcc::i2c::BUS_STANDBY;

		// Initiate a START condition; Interrupt enabled and flag cleared
		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);

		return true;
	}
	else
		return false;
}

void
xpcc::atmega::AsynchronousTwiMaster::restart(uint8_t slaveAddress)
{
	// Copy address; clear R/~W bit in SLA+R/W address field
	twiAddress = slaveAddress & 0xfe;
	busState = xpcc::i2c::BUS_STANDBY;

	if (!startConditionGenerated){
		// Initiate a START condition; Interrupt enabled and flag cleared
		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
	}
	else{// after busState is BUS_HOLD startcondition is generated, but restart has to be called
		// Enable interrupt
		TWCR |= (1 << TWIE);
	}
}

void
xpcc::atmega::AsynchronousTwiMaster::stop()
{
	if (busState != xpcc::i2c::BUS_STOPPED && busState != xpcc::i2c::BUS_RESET)
		TWCR |= (1 << TWSTO);
	occupied = false;
}

void
xpcc::atmega::AsynchronousTwiMaster::read(uint8_t *data, std::size_t size, xpcc::i2c::ReadParameter param)
{
	// prepare read operation
	twiReadBuffer = data;
	twiReadCounter = size;
	restartAfterReading = param == xpcc::i2c::READ_RESTART;
	reading = true;

	// Interrupt enabled and flag cleared
	TWCR = (1 << TWEN) | (1 << TWIE);
}

void
xpcc::atmega::AsynchronousTwiMaster::write(const uint8_t *data, std::size_t size)
{
	// Save pointer to data and number of bytes to send
	twiWriteBuffer = data;
	twiWriteCounter = size;
	reading = false;

	// Interrupt enabled
	TWCR = (1 << TWEN) | (1 << TWIE);
}

xpcc::i2c::BusyState
xpcc::atmega::AsynchronousTwiMaster::getBusyState()
{
	// If the TWI interrupt is enabled then the peripheral is busy
	uint8_t tmp = TWCR;
	if ((tmp & (1 << TWIE)) || (tmp & (1 << TWSTO)))
		return xpcc::i2c::BUSY;
	else
		return occupied?
				xpcc::i2c::OCCUPIED
				:xpcc::i2c::FREE;
}

xpcc::i2c::BusState
xpcc::atmega::AsynchronousTwiMaster::getBusState()
{
	return busState;
}

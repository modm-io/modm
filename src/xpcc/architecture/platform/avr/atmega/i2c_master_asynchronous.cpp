// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 * $Id: i2c_master_asynchronous.cpp 752 2012-03-03 23:54:03Z salkinium $
 */
// ----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <xpcc/architecture/driver/atomic/lock.hpp>

#include "i2c_master.hpp"

// ----------------------------------------------------------------------------
namespace
{
	// parameter advice
	static bool reading;
	static bool restartAfterReading;
	static bool restartAfterWriting;
	static uint8_t twiAddress;
	
	// buffer management
	static const uint8_t *twiWriteBuffer;
	static uint8_t *twiReadBuffer;
	static uint8_t twiReadCounter;
	static uint8_t twiWriteCounter;
	static uint8_t twiReadBufferSize;
	static uint8_t twiWriteBufferSize;
	
	// state
	static xpcc::i2c::BusState busState = xpcc::i2c::BUS_RESET;
	static xpcc::i2c::ErrorState errorState = xpcc::i2c::NO_ERROR;
	static bool occupied = false;
	static bool startConditionGenerated =  false;
	
	// delegating
	static xpcc::i2c::Delegate *delegate = 0;
}

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
			
			// Load data register with TWI slave address
			TWDR = twiAddress | reading;
			// clear interrupt flag to send address
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
			startConditionGenerated = false;
			break;
			
		case TW_MT_SLA_ACK:
			// SLA+W has been transmitted and ACK received
			// Fall through...
		case TW_MT_DATA_ACK:
			// Data byte has been transmitted and ACK received
			
			if (twiWriteCounter > 0)
			{
				// Load data register with next byte
				TWDR = *twiWriteBuffer++;
				twiWriteCounter--;
				twiWriteBufferSize++;
				// TWI Interrupt enabled and clear flag to send next byte
				TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
			}
			else
			{
				// Transfer finished
				if (twiReadCounter == 0)
				{
					if (restartAfterWriting) {
						// restart and strech SCL
						xpcc::atmega::AsynchronousI2cMaster::restart(twiAddress);
						// the bus is now reserved for further operations
						busState = xpcc::i2c::BUS_WRITE;
					}
					else xpcc::atmega::AsynchronousI2cMaster::stop();
					
					if (delegate) {
						xpcc::atomic::Unlock unlock;
						xpcc::i2c::Delegate *bufferedDelegate = delegate;
						delegate = 0;
						bufferedDelegate->twiCompletion(twiWriteBuffer-twiWriteBufferSize, twiWriteBufferSize, xpcc::i2c::WRITE);
					}
				}
				else
				{
					// restart, BUT do _not_ strech SCL, because we already
					// know the address of the slave and the operation
					xpcc::atmega::AsynchronousI2cMaster::restart(twiAddress);
					reading = true;
					// enable the interrupt, because we want to continue
					TWCR |= (1 << TWIE);
				}
			}
			break;
			
		case TW_MR_DATA_ACK:
			// Data byte has been received and ACK transmitted
			*twiReadBuffer++ = TWDR;
			twiReadCounter--;
			twiReadBufferSize++;
			
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
			twiReadBufferSize++;
			
			if (restartAfterReading) {
				// restart and strech SCL
				xpcc::atmega::AsynchronousI2cMaster::restart(twiAddress);
				// the bus is now reserved for further read operations
				busState = xpcc::i2c::BUS_HOLD;
			}
			else xpcc::atmega::AsynchronousI2cMaster::stop();
			
			if (delegate) {
				xpcc::atomic::Unlock unlock;
				xpcc::i2c::Delegate *bufferedDelegate = delegate;
				delegate = 0;
				bufferedDelegate->twiCompletion(twiReadBuffer-twiReadBufferSize, twiReadBufferSize, xpcc::i2c::READ);
			}
			break;
			
		case TW_MT_SLA_NACK:	// SLA+W transmitted, NACK received
		case TW_MR_SLA_NACK:	// SLA+R transmitted, NACK received
			errorState = xpcc::i2c::ADDRESS_NACK;
		case TW_MT_DATA_NACK:	// data transmitted, NACK received
			if (!errorState) errorState = xpcc::i2c::DATA_NACK;
			// generate a stop condition
			TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
		case TW_MT_ARB_LOST:	// arbitration lost in SLA+W or data
//		case TW_MR_ARB_LOST:	// arbitration lost in SLA+R or NACK
			if (!errorState) errorState = xpcc::i2c::ARBITRATION_LOST;
		default:
			if (!errorState) errorState = xpcc::i2c::UNKNOWN_ERROR;
			if (delegate) {
				xpcc::i2c::Delegate *bufferedDelegate = delegate;
				delegate = 0;
				bufferedDelegate->twiError(errorState);
			}
			// calling this after the callback to be able to get some extra
			// information of the error during the callback
			busState = xpcc::i2c::BUS_RESET;
			startConditionGenerated = false;
			occupied = false;
			break;
	}
}

// ----------------------------------------------------------------------------
void
xpcc::atmega::AsynchronousI2cMaster::initialize(uint8_t twbr, uint8_t twps)
{
	reset();
	// Initialize TWI clock
	TWBR = twbr;
	TWSR = twps & 0x03;
	// Load data register with default content; release SDA
	TWDR = 0xff;
	// Enable TWI peripheral with interrupt disabled
	TWCR = (1 << TWEN);
}

void
xpcc::atmega::AsynchronousI2cMaster::reset()
{
	twiReadCounter = 0;
	twiWriteCounter = 0;
	busState = xpcc::i2c::BUS_RESET;
	startConditionGenerated =  false;
	occupied = false;
	errorState = xpcc::i2c::NO_ERROR;
	delegate = 0;
}

// MARK: - ownership
bool
xpcc::atmega::AsynchronousI2cMaster::start(uint8_t slaveAddress)
{
	if (getBusyState() == xpcc::i2c::FREE)
	{
		// Copy address; clear R/~W bit in SLA+R/W address field
		twiAddress = slaveAddress & 0xfe;
		busState = xpcc::i2c::BUS_STANDBY;
		occupied = true;
		startConditionGenerated = true;
		errorState = xpcc::i2c::NO_ERROR;

		// Generate a START condition
		// BUT do _not_ enable the interrupt
		// This stretches the clock line, until the call of read/write
		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
		
		return true;
	}
	return false;
}

void
xpcc::atmega::AsynchronousI2cMaster::restart(uint8_t slaveAddress)
{
	if (!startConditionGenerated)
	{
		// Copy address; clear R/~W bit in SLA+R/W address field
		twiAddress = slaveAddress & 0xfe;
		busState = xpcc::i2c::BUS_STANDBY;
		occupied = true;
		startConditionGenerated = true;
		
		// Generate a START condition
		// BUT do _not_ enable the interrupt
		// This stretches the clock line, until the call of read/write
		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	}
}

void
xpcc::atmega::AsynchronousI2cMaster::stop()
{
	if (busState != xpcc::i2c::BUS_STOPPED && busState != xpcc::i2c::BUS_RESET)
	{
		// generate stop condition
		TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
		busState = xpcc::i2c::BUS_STOPPED;
		startConditionGenerated = false;
		occupied = false;
	}
}

// MARK: - operations
void
xpcc::atmega::AsynchronousI2cMaster::read(uint8_t *data, std::size_t size, xpcc::i2c::OperationParameter param)
{
	writeRead(data, 0, size, false, (param == xpcc::i2c::READ_RESTART));
}

void
xpcc::atmega::AsynchronousI2cMaster::write(uint8_t *data, std::size_t size, xpcc::i2c::OperationParameter param)
{
	writeRead(data, size, 0, (param == xpcc::i2c::WRITE_RESTART), false);
}

void
xpcc::atmega::AsynchronousI2cMaster::writeRead(uint8_t *data, std::size_t writeSize, std::size_t readSize, xpcc::i2c::OperationParameter param)
{
	writeRead(data, writeSize, readSize, false, (param == xpcc::i2c::READ_RESTART));
}

// MARK: - delegate
void
xpcc::atmega::AsynchronousI2cMaster::attachDelegate(xpcc::i2c::Delegate *object)
{
	delegate = object;
}

// MARK: - status
xpcc::i2c::ErrorState
xpcc::atmega::AsynchronousI2cMaster::getErrorState()
{
	return errorState;
}

xpcc::i2c::BusyState
xpcc::atmega::AsynchronousI2cMaster::getBusyState()
{
	// If the TWI interrupt is enabled then the peripheral is busy
	volatile uint8_t tmp = TWCR;
	if ((tmp & (1 << TWIE)) || (tmp & (1 << TWSTO)))
		return xpcc::i2c::BUSY;
	
	return occupied ? xpcc::i2c::OCCUPIED : xpcc::i2c::FREE;
}

xpcc::i2c::BusState
xpcc::atmega::AsynchronousI2cMaster::getBusState()
{
	return busState;
}

// MARK: - private
void
xpcc::atmega::AsynchronousI2cMaster::writeRead(uint8_t *data, std::size_t writeSize, std::size_t readSize, bool restartW, bool restartR)
{	
	// Save pointer to data and number of bytes to send
	twiWriteBuffer = data;
	twiReadBuffer = data;
	twiWriteCounter = writeSize;
	twiReadCounter = readSize;
	twiWriteBufferSize = 0;
	twiReadBufferSize = 0;
	
	restartAfterWriting = restartW;
	restartAfterReading = restartR;
	reading = writeSize ? false : true;
	
	// ONLY enable the interrupt, do _not_ clear the interrupt flag
	// Since we have streched the SCL, we will first write the address
	// of the slave, then continue with a write operation
	TWCR = (1 << TWEN) | (1 << TWIE);
}

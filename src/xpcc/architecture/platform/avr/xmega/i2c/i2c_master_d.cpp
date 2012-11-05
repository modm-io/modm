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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#include <xpcc/architecture/driver/atomic/lock.hpp>
#include "i2c_master_d.hpp"
// uncomment to debug your driver using simple uart
/*
#include "../uart.hpp"
#define DEBUG(x) xpcc::xmega::BufferedUartF0::write(x)
/*/
#define DEBUG(x)
//*/

#if defined(TWID)

// ----------------------------------------------------------------------------
namespace
{
	// parameter advice
	static xpcc::i2c::Delegate::NextOperation nextOperation;
	
	// buffer management
	static uint8_t *readPointer;
	static const uint8_t *writePointer;
	static std::size_t readBytesLeft;
	static std::size_t writeBytesLeft;
	
	// delegating
	static xpcc::i2c::Delegate *delegate = 0;
	static xpcc::xmega::I2cMasterD::ErrorState errorState(xpcc::xmega::I2cMasterD::NO_ERROR);
	
	// helper functions
	static void
	initializeWrite(xpcc::i2c::Delegate::Writing w) {
		writePointer = w.buffer;
		writeBytesLeft = w.size;
		nextOperation = static_cast<xpcc::i2c::Delegate::NextOperation>(w.next);
	}
	
	static void
	initializeRead(xpcc::i2c::Delegate::Reading r) {
		readPointer = r.buffer;
		readBytesLeft = r.size;
		nextOperation = static_cast<xpcc::i2c::Delegate::NextOperation>(r.next);
	}
	
	static void
	initializeStopAfterAddress() {
		//writePointer = ..;
		writeBytesLeft = 0;
		nextOperation = xpcc::i2c::Delegate::STOP_OP;
	}
	
	static void
	initializeRestartAfterAddress() {
		//writePointer = ..;
		writeBytesLeft = 0;
		nextOperation = xpcc::i2c::Delegate::RESTART_OP;
	}
	
	static void
	callStarting()
	{
		DEBUG('s');
		writeBytesLeft = 0;
		readBytesLeft = 0;
		TWID_MASTER_CTRLA &= ~(TWI_MASTER_RIEN_bm | TWI_MASTER_WIEN_bm);
		xpcc::i2c::Delegate::Starting s = delegate->starting();
		uint8_t address;
		switch (s.next)
		{
			case xpcc::i2c::Delegate::READ_OP:
				address = (s.address & 0xfe) | xpcc::i2c::READ;
				initializeRead(delegate->reading());
				break;
				
			case xpcc::i2c::Delegate::WRITE_OP:
				address = (s.address & 0xfe) | xpcc::i2c::WRITE;
				initializeWrite(delegate->writing());
				break;
				
			default:
			case xpcc::i2c::Delegate::STOP_OP:
				address = (s.address & 0xfe) | xpcc::i2c::WRITE;
				initializeStopAfterAddress();
				break;
				
			case xpcc::i2c::Delegate::RESTART_OP:
				address = (s.address & 0xfe) | xpcc::i2c::WRITE;
				initializeRestartAfterAddress();
				break;
		}
		// This also generates a start condition
		TWID_MASTER_ADDR = address;
		TWID_MASTER_CTRLA |= (address & xpcc::i2c::READ) ? TWI_MASTER_RIEN_bm : TWI_MASTER_WIEN_bm;
	}
}

// ----------------------------------------------------------------------------
/// TWI state machine interrupt handler
ISR(TWID_TWIM_vect)
{
	switch(TWID_MASTER_STATUS & TWI_MASTER_BUSSTATE_gm)
	{
		case TWI_MASTER_BUSSTATE_IDLE_gc:
			// Stop condition has been sent
			// chillax and get some coffee
			DEBUG('i');
			break;
			
		case TWI_MASTER_BUSSTATE_OWNER_gc:
			// We are writing
			if (TWID_MASTER_STATUS & TWI_MASTER_WIF_bm)
			{
				// Abitration and Bus Error is handled by the Busy state.
				
				// The Received Acknowledge flag must be checked for each data packet
				// transmitted before the next data packet can be transferred. The master
				// is not allowed to continue transmitting data if the slave signals a NACK.
				// 1 = NACK, 0 = ACK !!!
				if (TWID_MASTER_STATUS & TWI_MASTER_RXACK_bm)
				{
					DEBUG('N');
					errorState = xpcc::xmega::I2cMasterD::DATA_NACK;
					TWID_MASTER_CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
					xpcc::xmega::I2cMasterD::reset(true);
					// (M2)
					TWID_MASTER_STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
					break;
				}
				
				// Data byte has been transmitted and ACK received
				if (writeBytesLeft != 0)
				{
					// Writing the DATA register will trigger a data byte transfer, followed by
					// the master receiving the acknowledge bit from the slave.
					// The WIF and the CLKHOLD flag are set.
					// Accessing the DATA register will clear the master interrupt flags
					// and the CLKHOLD flag.
					TWID_MASTER_DATA = *writePointer++;
//					DEBUG('0' + writeBytesLeft);
					--writeBytesLeft;
					DEBUG('A');
				}
				else
				{
					switch (nextOperation)
					{
						case xpcc::i2c::Delegate::WRITE_OP:
							DEBUG('C');
							initializeWrite(delegate->writing());
							TWID_MASTER_DATA = *writePointer++;
							--writeBytesLeft;
							DEBUG('A');
							break;
							
						case xpcc::i2c::Delegate::RESTART_OP:
							DEBUG('R');
							TWID_MASTER_CTRLC = TWI_MASTER_CMD_REPSTART_gc;
							callStarting();
							break;
							
						default:
							DEBUG('S');
							TWID_MASTER_CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
							delegate->stopped(xpcc::i2c::Delegate::NORMAL_STOP);
							delegate = 0;
							break;
					}
				}
			}
			// We are reading
			else if (TWID_MASTER_STATUS & TWI_MASTER_RIF_bm)
			{
				// Accessing the DATA register will clear the master interrupt flags
				// and the CLKHOLD flag.
				*readPointer++ = TWID_MASTER_DATA;
//				DEBUG('0' + readBytesLeft);
				--readBytesLeft;
				
				// Indicating a NACK might not be successfully executed since arbitration
				// can be lost during the transmission.
				if (readBytesLeft > 0) {
					DEBUG('a');
					// Send ACK after reception
					TWID_MASTER_CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
				}
				else {
					DEBUG('n');
					switch (nextOperation)
					{
						case xpcc::i2c::Delegate::RESTART_OP:
							DEBUG('r');
							TWID_MASTER_CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_REPSTART_gc;
							callStarting();
							break;
							
						default:
						case xpcc::i2c::Delegate::STOP_OP:
							DEBUG('S');
							TWID_MASTER_CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
							delegate->stopped(xpcc::i2c::Delegate::NORMAL_STOP);
							delegate = 0;
							break;
					}
				}
			}
			break;
			
		case TWI_MASTER_BUSSTATE_BUSY_gc:
			// Arbitration Lost, or other master is using the bus
			// wait until bus status returns to IDLE
			// cancel all pending messages
			DEBUG('E');
			
			errorState = xpcc::xmega::I2cMasterD::ARBITRATION_LOST;
			xpcc::xmega::I2cMasterD::reset(true);
			
		default:
			TWID_MASTER_STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
			break;
	}
	DEBUG('\n');
}

// ----------------------------------------------------------------------------
void
xpcc::xmega::I2cMasterD::initialize(uint8_t baud, bool pullUpResistors)
{
	reset();
	
	// SDA, set as input
	PORTD_DIRCLR = (1 << 0);
	// SCL, set as output
	PORTD_DIRSET = (1 << 1);
	// set pull up resistors if wanted
	if (pullUpResistors) {
		PORTD_PIN0CTRL = PORT_OPC_PULLUP_gc;
		PORTD_PIN1CTRL = PORT_OPC_PULLUP_gc;
	}
	
	// SDA Hold Time and External Driver Interface disabled
	TWID_CTRL = 0;
	
	// set the baud rate register
	TWID_MASTER_BAUD = baud;
	
	// Enable the Master, and the interrupts
	TWID_MASTER_CTRLA = TWI_MASTER_INTLVL_MED_gc | TWI_MASTER_ENABLE_bm;
	// clear register b
	TWID_MASTER_CTRLB = 0;
	// clear register c
	TWID_MASTER_CTRLC = 0;
	
	// Set the bus state to idle
	TWID_MASTER_STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}

void
xpcc::xmega::I2cMasterD::reset(bool error)
{
	DEBUG('T');
	writeBytesLeft = 0;
	readBytesLeft = 0;
	if (delegate) delegate->stopped(error ? xpcc::i2c::Delegate::ERROR_CONDITION : xpcc::i2c::Delegate::SOFTWARE_RESET);
	delegate = 0;
}

uint8_t
xpcc::xmega::I2cMasterD::getErrorState()
{
	return static_cast<uint8_t>(errorState);
}

bool
xpcc::xmega::I2cMasterD::start(xpcc::i2c::Delegate *delegate)
{
	if (!::delegate && delegate && delegate->attaching())
	{
		::delegate = delegate;
		callStarting();
		
		return true;
	}
	return false;
}

bool
xpcc::xmega::I2cMasterD::startSync(xpcc::i2c::Delegate *delegate)
{
	if (!::delegate && delegate && delegate->attaching())
	{
		::delegate = delegate;
		callStarting();
		
		while (::delegate)
			;
		
		return true;
	}
	return false;
}

#endif
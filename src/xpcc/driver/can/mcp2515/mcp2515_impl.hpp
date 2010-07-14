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

#ifndef XPCC__MCP2515_HPP
	#error	"Don't include this file directly, use 'mcp2515.hpp' instead!"
#endif

#include "mcp2515_definitions.hpp"

namespace xpcc {
	namespace mcp2515 {
		EXTERN_FLASH(uint8_t configuration[24]);
	}
}

template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::initialize(Can::Bitrate bitrate)
{
	// software reset for the mcp2515, after this he is back in the
	// configuration mode
	CS::reset();
	SPI::write(SPI_RESET);
	delay_ms(1);
	CS::set();
	
	// wait a bit to give the mcp2515 some time to restart
	delay_ms(30);
	
	CS::reset();
	SPI::write(SPI_WRITE);
	SPI::write(CNF3);
	
	accessor::Flash<uint8_t> cfgPtr(mcp2515::configuration);
	for (uint8_t i = 0; i < 3; ++i)
	{
		SPI::write(cfgPtr[bitrate * 3 + i]);	// load CNF1..3
	}
	SPI::write((1 << RX1IE) | (1 << RX0IE));		// enable interrupts
	CS::set();
	
	writeRegister(TXRTSCTRL, 0);	// set TXnRTS pins as inwrites
	writeRegister(BFPCTRL, 0);		// disable RXnBF pins (high impedance state)

	// check if we could read back some of the values
	if (readRegister(CNF2) != cfgPtr[bitrate * 3 + 1])
	{
		// we are not able to communicate with the MCP2515 => abort
		return false;
	}
	
	// reset device to normal mode and disable the clkout pin and
	// wait until the new mode is active
	writeRegister(CANCTRL, 0);
	while ((readRegister(CANSTAT) & 0xe0) != 0)
		;
	
	return true;
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::setFilter(accessor::Flash<uint8_t> filter)
{
	// change to configuration mode
	bitModify(CANCTRL, 0xe0, (1<<REQOP2));
	while ((readRegister(CANSTAT) & 0xe0) != (1<<REQOP2))
		;
	
	writeRegister(RXB0CTRL, (1<<BUKT));
	writeRegister(RXB1CTRL, 0);
    
	uint8_t i, j;
	for (i = 0; i < 0x30; i += 0x10)
	{
		CS::reset();
		SPI::write(SPI_WRITE);
		SPI::write(i);
		
		for (j = 0; j < 12; j++) 
		{
			if (i == 0x20 && j >= 0x08)
				break;
			
			SPI::write(*filter++);
		}
		CS::set();
	}
	
	bitModify(CANCTRL, 0xe0, 0);
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::setMode(Can::Mode mode)
{
	uint8_t reg = 0;
	
	if (mode == Can::LISTEN_ONLY) {
		reg = (1<<REQOP1)|(1<<REQOP0);
	}
	else if (mode == Can::LOOPBACK) {
		reg = (1<<REQOP1);
	}
		
	// set the new mode
	bitModify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), reg);
	while ((readRegister(CANSTAT) & 0xe0) != reg) {
		// wait for the new mode to become active
	}
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::isMessageAvailable()
{
	return !INT::read();
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::getMessage(Can::Message& message)
{
	uint8_t addr;
	uint8_t status = readStatus(SPI_RX_STATUS);
	
	if (status & (1 << 6)) {
		addr = SPI_READ_RX;			// message in buffer 0
	}
	else if (status & (1 << 7)) {
		addr = SPI_READ_RX | 0x04;	// message in buffer 1
	}
	else {
		return false;				// Error: no message available
	}
	
	CS::reset();
	SPI::write(addr);
	
	message.flags.extended = readIdentifier(message.identifier);
	message.flags.rtr = (status & (1 << 3)) ? true : false;
	message.length = SPI::write(0xff) & 0x0f;
	
	for (uint8_t i = 0; i < message.length; ++i) {
		message.data[i] = SPI::write(0xff);
	}
	CS::set();
	
	// clear interrupt flag
	if (status & (1 << 6)) {
		bitModify(CANINTF, (1<<RX0IF), 0);
	}
	else {
		bitModify(CANINTF, (1<<RX1IF), 0);
	}
	
	return true;
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::isReadyToSend()
{
	if ((readStatus(SPI_READ_STATUS) & 0x54) == 0x54) {
		return false;		// all buffers used
	}
	else {
		return true;
	}
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::sendMessage(const Can::Message& message)
{
	uint8_t status = readStatus(SPI_READ_STATUS);
	
	// statusbyte:
	//
	// bit | function
	// ----+------------------
	//  2  | TXB0CNTRL.TXREQ
	//  4  | TXB1CNTRL.TXREQ
	//  6  | TXB2CNTRL.TXREQ
	uint8_t address;
	if ((status & (1 << 2)) == 0) {
		address = 0x00;
	}
	else if ((status & (1 << 4)) == 0) {
		address = 0x02;
	} 
	else if ((status & (1 << 6)) == 0) {
		address = 0x04;
	}
	else {
		// all buffer are in use => could not send the message
		return 0;
	}
	
	CS::reset();
	SPI::write(SPI_WRITE_TX | address);
	writeIdentifier(message.identifier, message.flags.extended);
	
	// if the message is a rtr-frame, is has a length but no attached data
	if (message.flags.rtr)
	{
		SPI::write((1 << RTR) | message.length);
	}
	else
	{
		SPI::write(message.length);
		
		for (uint8_t i = 0; i < message.length; ++i)
		{
			SPI::write(message.data[i]);
		}
	}
	CS::set();
	
	delay_us(1);
	
	// send message via RTS command
	CS::reset();
	address = (address == 0) ? 1 : address;	// 0 2 4 => 1 2 4
	SPI::write(SPI_RTS | address);
	CS::set();
	
	return address;
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::writeRegister(uint8_t address, uint8_t data)
{
	CS::reset();
	
	SPI::write(SPI_WRITE);
	SPI::write(address);
	SPI::write(data);
	
	CS::set();
}

template <typename SPI, typename CS, typename INT>
uint8_t
xpcc::Mcp2515<SPI, CS, INT>::readRegister(uint8_t address)
{
	uint8_t data;
	
	CS::reset();
	
	SPI::write(SPI_READ);
	SPI::write(address);
	data = SPI::write(0xff);
	
	CS::set();
	
	return data;
}

template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::bitModify(uint8_t address, uint8_t mask, uint8_t data)
{
	CS::reset();
	
	SPI::write(SPI_BIT_MODIFY);
	SPI::write(address);
	SPI::write(mask);
	SPI::write(data);
	
	CS::set();
}

template <typename SPI, typename CS, typename INT>
uint8_t
xpcc::Mcp2515<SPI, CS, INT>::readStatus(uint8_t type)
{
	uint8_t data;
	
	CS::reset();
	
	SPI::write(type);
	data = SPI::write(0xff);
	
	CS::set();
	
	return data;
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::writeIdentifier(const uint32_t& identifier,
											 bool isExtendedFrame)
{
	const uint32_t *ptr = &identifier;
	
	if (isExtendedFrame)
	{
		uint8_t tmp;
		
		SPI::write(*((uint16_t *) ptr + 1) >> 5);
		
		// calculate the next values
		tmp  = (*((uint8_t *) ptr + 2) << 3) & 0xe0;
		tmp |= (1 << IDE);
		tmp |= (*((uint8_t *) ptr + 2)) & 0x03;
		
		SPI::write(tmp);
		SPI::write(*((uint8_t *) ptr + 1));
		SPI::write(*((uint8_t *) ptr));
	}
	else
	{
		SPI::write(*((uint16_t *) ptr) >> 3);
		SPI::write(*((uint8_t *) ptr) << 5);
		SPI::write(0);
		SPI::write(0);
	}
}

template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::readIdentifier(uint32_t& identifier)
{
	uint32_t *ptr = &identifier;
	
	uint8_t first = SPI::write(0xff);
	uint8_t tmp   = SPI::write(0xff);
	
	if (tmp & (1 << IDE))
	{
		*((uint16_t *) ptr + 1)  = (uint16_t) first << 5;
		*((uint8_t *)  ptr + 1)  = SPI::write(0xff);
		
		*((uint8_t *)  ptr + 2) |= (tmp >> 3) & 0x1C;
		*((uint8_t *)  ptr + 2) |=  tmp & 0x03;
		
		*((uint8_t *)  ptr)      = SPI::write(0xff);
		
		return true;
	}
	else {
		SPI::write(0xff);
		
		*((uint8_t *)  ptr + 3) = 0;
		*((uint8_t *)  ptr + 2) = 0;
		
		*((uint16_t *) ptr) = (uint16_t) first << 3;
		
		SPI::write(0xff);
		
		*((uint8_t *) ptr) |= tmp >> 5;
		
		return false;
	}
}

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

#ifndef XPCC__MCP2515_HPP
	#error	"Don't include this file directly, use 'mcp2515.hpp' instead!"
#endif

#include "mcp2515_definitions.hpp"

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
SPI xpcc::Mcp2515<SPI, CS, INT>::spi;

template <typename SPI, typename CS, typename INT>
CS xpcc::Mcp2515<SPI, CS, INT>::chipSelect;

template <typename SPI, typename CS, typename INT>
INT xpcc::Mcp2515<SPI, CS, INT>::interruptPin;

// ----------------------------------------------------------------------------
namespace xpcc
{
	namespace mcp2515
	{
		EXTERN_FLASH_STORAGE(uint8_t configuration[24]);
	}
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::initialize(uint32_t bitrate)
{
	using namespace mcp2515;

	// software reset for the mcp2515, after this the chip is back in the
	// configuration mode
	chipSelect.reset();
	spi.transferBlocking(RESET);
	xpcc::delayMilliseconds(1);
	chipSelect.set();

	// wait a bit to give the MCP2515 some time to restart
	xpcc::delayMilliseconds(30);

	chipSelect.reset();
	spi.transferBlocking(WRITE);
	spi.transferBlocking(CNF3);

	accessor::Flash<uint8_t> cfgPtr(mcp2515::configuration);
	for (uint8_t i = 0; i < 3; ++i)
	{
		// load CNF1..3
		spi.transferBlocking(cfgPtr[static_cast<uint8_t>(bitrate) * 3 + i]);
	}

	// enable interrupts
	spi.transferBlocking(RX1IE | RX0IE);
	chipSelect.set();

	// set TXnRTS pins as inwrites
	writeRegister(TXRTSCTRL, 0);

	// disable RXnBF pins (high impedance state)
	writeRegister(BFPCTRL, 0);

	// check if we could read back some of the values
	if (readRegister(CNF2) != cfgPtr[static_cast<uint8_t>(bitrate) * 3 + 1])
	{
		// we are not able to communicate with the MCP2515 => abort
		return false;
	}

	// reset device to normal mode and disable the clkout pin and
	// wait until the new mode is active
	writeRegister(CANCTRL, 0);
	while ((readRegister(CANSTAT) &
			(OPMOD2 | OPMOD1 | OPMOD0)) != 0) {
	}

	return true;
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::setFilter(accessor::Flash<uint8_t> filter)
{
	using namespace mcp2515;

	// change to configuration mode
	bitModify(CANCTRL, 0xe0, REQOP2);
	while ((readRegister(CANSTAT) & 0xe0) != REQOP2)
		;

	writeRegister(RXB0CTRL, BUKT);
	writeRegister(RXB1CTRL, 0);

	uint8_t i, j;
	for (i = 0; i < 0x30; i += 0x10)
	{
		chipSelect.reset();
		spi.transferBlocking(WRITE);
		spi.transferBlocking(i);

		for (j = 0; j < 12; j++)
		{
			if (i == 0x20 && j >= 0x08)
				break;

			spi.transferBlocking(*filter++);
		}
		chipSelect.set();
	}

	bitModify(CANCTRL, 0xe0, 0);
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::setMode(Can::Mode mode)
{
	using namespace mcp2515;

	uint8_t reg = 0;
	if (mode == Can::Mode::ListenOnly) {
		reg = REQOP1 | REQOP0;
	}
	else if (mode == Can::Mode::LoopBack) {
		reg = REQOP1;
	}

	// set the new mode
	bitModify(CANCTRL, REQOP2 | REQOP1 | REQOP0, reg);

	while ((readRegister(CANSTAT) &	(OPMOD2 | OPMOD1 | OPMOD0)) != reg) {
		// wait for the new mode to become active
	}
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::isMessageAvailable()
{
	return !interruptPin.read();
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::getMessage(can::Message& message)
{
	using namespace mcp2515;

	uint8_t status = readStatus(RX_STATUS);
	uint8_t address;
	if (status & FLAG_RXB0_FULL) {
		address = READ_RX;			// message in buffer 0
	}
	else if (status & FLAG_RXB1_FULL) {
		address = READ_RX | 0x04;	// message in buffer 1 (RXB1SIDH)
	}
	else {
		return false;				// Error: no message available
	}

	chipSelect.reset();
	spi.transferBlocking(address);

	message.flags.extended = readIdentifier(message.identifier);
	if (status & FLAG_RTR) {
		message.flags.rtr = true;
	}
	else {
		message.flags.rtr = false;
	}
	message.length = spi.transferBlocking(0xff) & 0x0f;

	for (uint8_t i = 0; i < message.length; ++i) {
		message.data[i] = spi.transferBlocking(0xff);
	}
	chipSelect.set();

	// clear interrupt flag
	if (status & FLAG_RXB0_FULL) {
		bitModify(CANINTF, RX0IF, 0);
	}
	else {
		bitModify(CANINTF, RX1IF, 0);
	}

	return true;
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::isReadyToSend()
{
	using namespace mcp2515;

	if ((readStatus(READ_STATUS) & (TXB2CNTRL_TXREQ | TXB1CNTRL_TXREQ | TXB0CNTRL_TXREQ)) ==
			(TXB2CNTRL_TXREQ | TXB1CNTRL_TXREQ | TXB0CNTRL_TXREQ))
	{
		// all buffers currently in use
		return false;
	}
	else {
		return true;
	}
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::sendMessage(const can::Message& message)
{
	using namespace mcp2515;

	uint8_t status = readStatus(READ_STATUS);
	uint8_t address;
	if ((status & TXB0CNTRL_TXREQ) == 0) {
		address = 0x00;		// TXB0SIDH
	}
	else if ((status & TXB1CNTRL_TXREQ) == 0) {
		address = 0x02;		// TXB1SIDH
	}
	else if ((status & TXB2CNTRL_TXREQ) == 0) {
		address = 0x04;		// TXB2SIDH
	}
	else {
		// all buffer are in use => could not send the message
		return 0;
	}

	chipSelect.reset();
	spi.transferBlocking(WRITE_TX | address);
	writeIdentifier(message.identifier, message.flags.extended);

	// if the message is a rtr-frame, is has a length but no attached data
	if (message.flags.rtr) {
		spi.transferBlocking(MCP2515_RTR | message.length);
	}
	else {
		spi.transferBlocking(message.length);

		for (uint8_t i = 0; i < message.length; ++i) {
			spi.transferBlocking(message.data[i]);
		}
	}
	chipSelect.set();

	xpcc::delayMicroseconds(1);

	// send message via RTS command
	chipSelect.reset();
	address = (address == 0) ? 1 : address;	// 0 2 4 => 1 2 4
	spi.transferBlocking(RTS | address);
	chipSelect.set();

	return address;
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::writeRegister(uint8_t address, uint8_t data)
{
	chipSelect.reset();

	spi.transferBlocking(WRITE);
	spi.transferBlocking(address);
	spi.transferBlocking(data);

	chipSelect.set();
}

template <typename SPI, typename CS, typename INT>
uint8_t
xpcc::Mcp2515<SPI, CS, INT>::readRegister(uint8_t address)
{
	chipSelect.reset();

	spi.transferBlocking(READ);
	spi.transferBlocking(address);
	uint8_t data = spi.transferBlocking(0xff);

	chipSelect.set();

	return data;
}

template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::bitModify(uint8_t address, uint8_t mask, uint8_t data)
{
	chipSelect.reset();

	spi.transferBlocking(BIT_MODIFY);
	spi.transferBlocking(address);
	spi.transferBlocking(mask);
	spi.transferBlocking(data);

	chipSelect.set();
}

template <typename SPI, typename CS, typename INT>
uint8_t
xpcc::Mcp2515<SPI, CS, INT>::readStatus(uint8_t type)
{
	chipSelect.reset();

	spi.transferBlocking(type);
	uint8_t data = spi.transferBlocking(0xff);

	chipSelect.set();

	return data;
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename INT>
void
xpcc::Mcp2515<SPI, CS, INT>::writeIdentifier(const uint32_t& identifier,
											 bool isExtendedFrame)
{
	using namespace mcp2515;

	const uint32_t *ptr = &identifier;

	if (isExtendedFrame)
	{
		spi.transferBlocking(*((uint16_t *) ptr + 1) >> 5);

		// calculate the next values
		uint8_t temp;
		temp  = (*((uint8_t *) ptr + 2) << 3) & 0xe0;
		temp |= MCP2515_IDE;
		temp |= (*((uint8_t *) ptr + 2)) & 0x03;

		spi.transferBlocking(temp);
		spi.transferBlocking(*((uint8_t *) ptr + 1));
		spi.transferBlocking(*((uint8_t *) ptr));
	}
	else
	{
		spi.transferBlocking(*((uint16_t *) ptr) >> 3);
		spi.transferBlocking(*((uint8_t *) ptr) << 5);
		spi.transferBlocking(0);
		spi.transferBlocking(0);
	}
}

template <typename SPI, typename CS, typename INT>
bool
xpcc::Mcp2515<SPI, CS, INT>::readIdentifier(uint32_t& identifier)
{
	using namespace mcp2515;

	uint32_t *ptr = &identifier;

	uint8_t first  = spi.transferBlocking(0xff);
	uint8_t second = spi.transferBlocking(0xff);

	if (second & MCP2515_IDE)
	{
		*((uint16_t *) ptr + 1)  = (uint16_t) first << 5;
		*((uint8_t *)  ptr + 1)  = spi.transferBlocking(0xff);

		*((uint8_t *)  ptr + 2) |= (second >> 3) & 0x1C;
		*((uint8_t *)  ptr + 2) |=  second & 0x03;

		*((uint8_t *)  ptr)      = spi.transferBlocking(0xff);

		return true;
	}
	else
	{
		spi.transferBlocking(0xff);

		*((uint8_t *)  ptr + 3) = 0;
		*((uint8_t *)  ptr + 2) = 0;

		*((uint16_t *) ptr) = (uint16_t) first << 3;

		spi.transferBlocking(0xff);

		*((uint8_t *) ptr) |= second >> 5;

		return false;
	}
}

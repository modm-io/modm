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

#ifndef XPCC__MCP23S17_HPP
	#error	"Don't include this file directly, use 'mcp23s17.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
Spi xpcc::Mcp23s17<Spi, Cs, Int>::spi;

template <typename Spi, typename Cs, typename Int>
Cs xpcc::Mcp23s17<Spi, Cs, Int>::cs;

template <typename Spi, typename Cs, typename Int>
Int xpcc::Mcp23s17<Spi, Cs, Int>::interrupt;

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
void
xpcc::Mcp23s17<Spi, Cs, Int>::initialize()
{
	cs.set();
	
	xpcc::delayMicroseconds(1);
	
	// Disable address pins (as they are by default) and enable the
	// open-drain output from the interrupt line. INTA and INTB mirrored.
	cs.reset();
	spi.writeReadBlocking(deviceAddress | WRITE);
	spi.writeReadBlocking(MCP_IOCON);
	spi.writeReadBlocking((1 << 6) | (1 << 2));
	spi.writeReadBlocking((1 << 6) | (1 << 2));
	cs.set();
	
	xpcc::delayMicroseconds(1);
}

template <typename Spi, typename Cs, typename Int>
void
xpcc::Mcp23s17<Spi, Cs, Int>::configure(uint16_t inputMask, uint16_t pullupMask)
{
	cs.reset();
	spi.writeReadBlocking(deviceAddress | WRITE);
	spi.writeReadBlocking(MCP_IODIR);
	spi.writeReadBlocking(inputMask & 0xff);
	spi.writeReadBlocking(inputMask >> 8);
	cs.set();
	
	xpcc::delayMicroseconds(1);
	
	cs.reset();
	spi.writeReadBlocking(deviceAddress | WRITE);
	spi.writeReadBlocking(MCP_GPPU);
	spi.writeReadBlocking(pullupMask & 0xff);
	spi.writeReadBlocking(pullupMask >> 8);
	cs.set();
	
	xpcc::delayMicroseconds(1);
}

//void
//configureInterrupt();

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
uint16_t
xpcc::Mcp23s17<Spi, Cs, Int>::read()
{
	cs.reset();
	spi.writeReadBlocking(deviceAddress | READ);
	spi.writeReadBlocking(MCP_GPIO);
	
	uint16_t value = spi.writeReadBlocking(0x00);
	value |= spi.writeReadBlocking(0x00) << 8;
	cs.set();
	
	xpcc::delayMicroseconds(0.1);
	
	return value;
}

template <typename Spi, typename Cs, typename Int>
void
xpcc::Mcp23s17<Spi, Cs, Int>::write(uint16_t output)
{
	cs.reset();
	spi.writeReadBlocking(deviceAddress | WRITE);
	spi.writeReadBlocking(MCP_GPIO);
	spi.writeReadBlocking(output & 0xff);
	spi.writeReadBlocking(output >> 8);
	cs.set();
	
	xpcc::delayMicroseconds(0.1);
}

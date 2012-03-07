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
 * $Id: mcp23s08_impl.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__MCP23S08_HPP
	#error	"Don't include this file directly, use 'mcp23s08.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
Spi xpcc::Mcp23s08<Spi, Cs, Int>::spi;

template <typename Spi, typename Cs, typename Int>
Cs xpcc::Mcp23s08<Spi, Cs, Int>::cs;

template <typename Spi, typename Cs, typename Int>
Int xpcc::Mcp23s08<Spi, Cs, Int>::interrupt;

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
void
xpcc::Mcp23s08<Spi, Cs, Int>::initialize()
{
	spi.initialize();
	cs.set();
	cs.setOutput();
	interrupt.setInput();
	
	xpcc::delay_us(0.1);
	
	// disable address pins (as they are by default) and enable the
	// open-drain output from the interrupt line
	cs.reset();
	spi.write(deviceAddress | WRITE);
	spi.write(MCP_IOCON);
	spi.write(1 << 2);
	cs.set();
	
	xpcc::delay_us(0.1);
}

template <typename Spi, typename Cs, typename Int>
void
xpcc::Mcp23s08<Spi, Cs, Int>::configure(uint8_t inputMask, uint8_t pullupMask)
{
	cs.reset();
	spi.write(deviceAddress | WRITE);
	spi.write(MCP_IODIR);
	spi.write(inputMask);
	cs.set();
	
	xpcc::delay_us(0.1);
	
	cs.reset();
	spi.write(deviceAddress | WRITE);
	spi.write(MCP_GPPU);
	spi.write(pullupMask);
	cs.set();
	
	xpcc::delay_us(0.1);
}

//void
//configureInterrupt();

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
uint8_t
xpcc::Mcp23s08<Spi, Cs, Int>::read()
{
	cs.reset();
	spi.write(deviceAddress | READ);
	spi.write(MCP_GPIO);
	
	uint8_t value = spi.write(0x00);
	cs.set();
	
	xpcc::delay_us(0.1);
	
	return value;
}

template <typename Spi, typename Cs, typename Int>
void
xpcc::Mcp23s08<Spi, Cs, Int>::write(uint8_t output)
{
	cs.reset();
	spi.write(deviceAddress | WRITE);
	spi.write(MCP_GPIO);
	spi.write(output);
	cs.set();
	
	xpcc::delay_us(0.1);
}

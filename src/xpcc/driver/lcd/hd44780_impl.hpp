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

#ifndef XPCC__HD44780_HPP
	#error	"Don't include this file directly, use 'hd44780.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template <typename E, typename RW, typename RS, typename DATA>
xpcc::Hd447800<E, RW, RS, DATA>::Hd447800() : 
	Lcd()
{
}

// ----------------------------------------------------------------------------

template <typename E, typename RW, typename RS, typename DATA>
void
xpcc::Hd447800<E, RW, RS, DATA>::initialize()
{
	E::setOutput();
	RW::setOutput();
	RS::setOutput();
	DATA::setOutput();
	
	E::reset();
	RS::reset();
	xpcc::delay_ms(15);
	
	writeNibble(0x03);
	xpcc::delay_ms(5);
	
	writeNibble(0x03);
	xpcc::delay_us(100);
	
	writeNibble(0x03);
	xpcc::delay_us(100);
	
	writeNibble(0x02);
	xpcc::delay_us(100);
	
	writeCommand(0x28);		// 2 lines 5*7
	writeCommand(0x08);		// display off
	writeCommand(0x01);		// display clear
	writeCommand(0x06);		// cursor increment
	writeCommand(0x0C);		// on, no cursor, no blink
}

template <typename E, typename RW, typename RS, typename DATA>
void
xpcc::Hd447800<E, RW, RS, DATA>::writeRaw(char c)
{
	waitBusy();
	
	DATA::setOutput();
	RW::reset();
	RS::set();
	
	uint8_t data = c;
	writeNibble(data >> 4);
	writeNibble(data);
}

template <typename E, typename RW, typename RS, typename DATA>
void
xpcc::Hd447800<E, RW, RS, DATA>::setPosition(uint8_t line, uint8_t column)
{
	this->column = column;
	this->line = line;
	
	column += 0x40 * line;
	writeCommand(0x80 | column);
}

// ----------------------------------------------------------------------------

template <typename E, typename RW, typename RS, typename DATA>
void
xpcc::Hd447800<E, RW, RS, DATA>::writeNibble(uint8_t data)
{
	DATA::write(data);
	
	E::set();
	xpcc::delay_us(1);
	E::reset();
}

template <typename E, typename RW, typename RS, typename DATA>
uint8_t
xpcc::Hd447800<E, RW, RS, DATA>::readByte()
{
	uint8_t data;
	
	DATA::setInput();
	RS::reset();
	RW::set();
	
	E::set();
	xpcc::delay_us(1);
	data = DATA::read();
	E::reset();
	
	data <<= 4;
	
	E::set();
	xpcc::delay_us(1);
	data |= DATA::read();
	E::reset();
	
	return data;
}

template <typename E, typename RW, typename RS, typename DATA>
void
xpcc::Hd447800<E, RW, RS, DATA>::waitBusy()
{
	while (readByte() & 0x80) {
		// wait until busy flag is reseted
	}
	
	// the address counter is updated 4 us after the busy flag is reseted
	xpcc::delay_us(2);
}

template <typename E, typename RW, typename RS, typename DATA>
void
xpcc::Hd447800<E, RW, RS, DATA>::writeCommand(uint8_t command)
{
	waitBusy();
	
	DATA::setOutput();
	RW::reset();
	RS::reset();
	
	writeNibble(command >> 4);
	writeNibble(command);
}

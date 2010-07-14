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

#ifndef XPCC__DOG_M16X_HPP
	#error	"Don't include this file directly, use 'dog_m16x.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

namespace xpcc {
	namespace dog_m16x {
		EXTERN_FLASH(uint8_t configuration[10]);
	}
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS>
xpcc::DogM16x<SPI, CS, RS>::DogM16x() :
	Lcd()
{
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS>
void
xpcc::DogM16x<SPI, CS, RS>::initialize()
{
	SPI::initialize();
	CS::setOutput();
	RS::setOutput();
	
	accessor::Flash<uint8_t> config(dog_m16x::configuration);
	for (uint8_t i = 0; i < 10; ++i)
	{
		writeCommand(config[i]);
	}
}

template <typename SPI, typename CS, typename RS>
void
xpcc::DogM16x<SPI, CS, RS>::putRaw(char c)
{
	RS::set();
	
	CS::reset();
	SPI::write(c);
	CS::set();
}

template <typename SPI, typename CS, typename RS>
void
xpcc::DogM16x<SPI, CS, RS>::setPosition(uint8_t line, uint8_t column)
{
	this->column = column;
	this->line = line;
	
	column += 0x40 * line;
	writeCommand(0x80 | column);
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS>
void
xpcc::DogM16x<SPI, CS, RS>::writeCommand(uint8_t command)
{
	RS::reset();
	
	CS::reset();
	SPI::write(command);
	CS::set();
	
	// check if the command is 'clear display' oder 'return home', these
	// commands take a bit longer until they are finished.
	if ((command & 0xfc) == 0) {
		delay_ms(1.2);
	}
	else {
		delay_us(27);
	}
}

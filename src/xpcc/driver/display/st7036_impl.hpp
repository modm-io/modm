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

#ifndef XPCC__ST7036_HPP
	#error	"Don't include this file directly, use 'st7036.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

namespace xpcc {
	namespace st7036 {
		EXTERN_FLASH_STORAGE(uint8_t configuration[10]);
	}
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
xpcc::St7036<SPI, CS, RS, Width, Heigth>::St7036() :
	CharacterDisplay(Width, Heigth)
{
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::initialize()
{
	accessor::Flash<uint8_t> config(st7036::configuration);
	for (uint8_t i = 0; i < 10; ++i)
	{
		writeCommand(config[i]);
	}
}

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::writeRaw(char c)
{
	RS::set();
	
	CS::reset();
	SPI::writeReadBlocking(c);
	CS::set();
}

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::execute(Command command)
{
	writeCommand(command);
}

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::setCursor(uint8_t newLine, uint8_t newColumn)
{
	this->column = newColumn;
	this->line = newLine;
	
	newColumn += 0x40 * newLine;
	writeCommand(0x80 | newColumn);
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::writeCommand(uint8_t inCommand)
{
	RS::reset();
	
	CS::reset();
	SPI::writeReadBlocking(inCommand);
	CS::set();
	
	// check if the command is 'clear display' oder 'return home', these
	// commands take a bit longer until they are finished.
	if ((inCommand & 0xfc) == 0) {
		xpcc::delayMicroseconds(1200);
	}
	else {
		xpcc::delayMicroseconds(27);
	}
}

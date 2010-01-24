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
 * $Id: iostream.cpp 64 2009-09-27 15:19:16Z thundernail $
 */
// ----------------------------------------------------------------------------

#include "iostream.hpp"

#include <xpcc/hal/flash/flash_pointer.hpp>

FLASH(uint16_t base[]) = { 10, 100, 1000, 10000 };


// ----------------------------------------------------------------------------
xpcc::IOStream::IOStream( IODevice* device ) :
	device( device ),
	mode ( ASCII )
{
}

// ----------------------------------------------------------------------------

void
xpcc::IOStream::putInteger(int16_t value)
{
	FlashPointer<uint16_t> basePtr = xpcc::toFlashPointer(base);
	
	if (value < 0) {
		value = -value;
		this->device->put('-');
	}
	
	bool zero = true;
	uint8_t i = 4;
	do {
		i--;
		char d;
		for (d = '0'; value >= (int16_t) basePtr[i]; value -= basePtr[i])
		{
			d++;
			zero = false;
		}
		if (!zero) {
			this->device->put(d);
		}
	} while (i);
	
	this->device->put(static_cast<char>(value) + '0');
}

// ----------------------------------------------------------------------------
void
xpcc::IOStream::putInteger(uint16_t value)
{
	bool zero = true;
	FlashPointer<uint16_t> basePtr = xpcc::toFlashPointer(base);

	uint8_t i = 4;
	do {
		i--;
		char d;
		for (d = '0'; value >= basePtr[i]; value -= basePtr[i])
		{
			d++;
			zero = false;
		}
		if (!zero) {
			this->device->put(d);
		}
	} while (i);
	
	this->device->put(static_cast<char>(value) + '0');
}

// ----------------------------------------------------------------------------
#ifdef __AVR__
	xpcc::IOStream&
	xpcc::IOStream::putInteger(int32_t value)
	{
		// -2,147,483,648 to 2,147,483,647
		char buffer[12];
		
		this->device->put(ltoa(value, buffer, 10));
		
		return *this;
	}
	
	xpcc::IOStream&
	xpcc::IOStream::putInteger(uint32_t value)
	{
		// 0 to 4,294,967,295
		char buffer[11];
		
		this->device->put(ultoa(value, buffer, 10));
		
		return *this;
	}

#endif

// ----------------------------------------------------------------------------

void
xpcc::IOStream::putHex( const char* s )
{
	while ( s != '\0' ) {
		this->putHex( *s );
		s++;
	}
}

// ----------------------------------------------------------------------------

void
xpcc::IOStream::putHex( char value )
{
	char nibble = (value >> 4);
	char hex;

	if ( nibble > 9 ) {
		hex = nibble+'A'-10;
	}
	else {
		hex = nibble+'0';
	}

	this->device->put(hex);

	nibble = (value & 0xF);
	if ( nibble > 9 ) {
		hex = nibble+'A'-10;
	}
	else {
		hex = nibble+'0';
	}

	this->device->put(hex);
}

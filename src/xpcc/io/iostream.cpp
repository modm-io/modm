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

#include <stdlib.h>
#include <xpcc/architecture/driver/accessor/flash.hpp>

#include "iostream.hpp"

FLASH_STORAGE(uint16_t base[]) = { 10, 100, 1000, 10000 };

// ----------------------------------------------------------------------------
xpcc::IOStream::IOStream(IODevice& device) :
	device(&device),
	mode(ASCII)
{
}

// ----------------------------------------------------------------------------
void
xpcc::IOStream::writeInteger(int16_t value)
{
	if (value < 0) {
		value = -value;
		this->device->write('-');
	}
	
	this->writeInteger(static_cast<uint16_t>(value));
}

void
xpcc::IOStream::writeInteger(uint16_t value)
{
	accessor::Flash<uint16_t> basePtr = xpcc::accessor::asFlash(base);
	
	bool zero = true;
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
			this->device->write(d);
		}
	} while (i);
	
	this->device->write(static_cast<char>(value) + '0');
}

void
xpcc::IOStream::writeInteger(int32_t value)
{
#if defined(XPCC__CPU_AVR)
	// Uses the optimized non standard function 'ltoa()' which is
	// not always available.
	char buffer[ArithmeticTraits<int32_t>::decimalDigits + 1]; // +1 for '\0'
	this->device->write(ltoa(value, buffer, 10));
#else
	char buffer[ArithmeticTraits<uint32_t>::decimalDigits + 1]; // +1 for '\0'
	// ptr points to the end of the string, it will be filled backwards
	char *ptr = buffer + ArithmeticTraits<uint32_t>::decimalDigits;
	*ptr = '\0';
	
	if (value < 0)
	{
		this->device->write('-');
		
		// calculate the string backwards
		ldiv_t qrem = { value, 0 };
		do {
			qrem = ldiv(qrem.quot, 10);
			*(--ptr) = '0' - qrem.rem;
		}
		while (qrem.quot != 0);
	}
	else
	{
		// calculate the string backwards
		ldiv_t qrem = { value, 0 };
		do {
			qrem = ldiv(qrem.quot, 10);
			*(--ptr) = '0' + qrem.rem;
		}
		while (qrem.quot != 0);
	}
	
	// write string
	this->device->write(ptr);
#endif
}

void
xpcc::IOStream::writeInteger(uint32_t value)
{
	char buffer[ArithmeticTraits<uint32_t>::decimalDigits + 1]; // +1 for '\0'
	
#if defined(XPCC__CPU_AVR)
	// Uses the optimized non standard function 'ultoa()' which is
	// not always available.
	this->device->write(ultoa(value, buffer, 10));
#elif defined(XPCC__CPU_ARM)
	snprintf(buffer, sizeof(buffer), "%lu", value);
	this->device->write(buffer);
#else
	snprintf(buffer, sizeof(buffer), "%u", value);
	this->device->write(buffer);
#endif
}

#ifndef XPCC__CPU_AVR
void
xpcc::IOStream::writeInteger(int64_t value)
{
	char buffer[ArithmeticTraits<int64_t>::decimalDigits + 1]; // +1 for '\0'
	
#if __SIZEOF_POINTER__ == 8
	snprintf(buffer, sizeof(buffer), "%ld", value);
#else
	snprintf(buffer, sizeof(buffer), "%lld", value);
#endif
	this->device->write(buffer);
}

void
xpcc::IOStream::writeInteger(uint64_t value)
{
	char buffer[ArithmeticTraits<uint64_t>::decimalDigits + 1]; // +1 for '\0'
	
#if __SIZEOF_POINTER__ == 8
	snprintf(buffer, sizeof(buffer), "%lu", value);
#else
	snprintf(buffer, sizeof(buffer), "%llu", value);
#endif
	this->device->write(buffer);
}
#endif

// ----------------------------------------------------------------------------
void
xpcc::IOStream::writeHex(const char* s)
{
	while (*s != '\0') {
		this->writeHex(*s);
		s++;
	}
}

// ----------------------------------------------------------------------------
void
xpcc::IOStream::writeHexNibble(uint8_t nibble)
{
	char hex;
	if (nibble > 9) {
		hex = nibble + 'A' - 10;
	}
	else {
		hex = nibble + '0';
	}
	this->device->write(hex);
}

// ----------------------------------------------------------------------------
void
xpcc::IOStream::writeHex(uint8_t value)
{
	writeHexNibble(value >> 4);
	writeHexNibble(value & 0xF);
}

// ----------------------------------------------------------------------------
xpcc::IOStream&
xpcc::IOStream::operator << (const void* p)
{
#if __SIZEOF_POINTER__ == 2
	
	this->device->write('0');
	this->device->write('x');
	
	uint16_t value = reinterpret_cast<uint16_t>(p);
	
	writeHex(value >> 8);
	writeHex(value);
	
#elif __SIZEOF_POINTER__ == 4
	
	this->device->write('0');
	this->device->write('x');
	
	uint32_t value = reinterpret_cast<uint32_t>(p);
	
	writeHex(value >> 24);
	writeHex(value >> 16);
	writeHex(value >> 8);
	writeHex(value);
	
#elif __SIZEOF_POINTER__ == 8
	
	this->device->write('0');
	this->device->write('x');
	
	uint64_t value = reinterpret_cast<uint64_t>(p);
	
	writeHex(value >> 56);
	writeHex(value >> 48);
	writeHex(value >> 40);
	writeHex(value >> 32);
	writeHex(value >> 24);
	writeHex(value >> 16);
	writeHex(value >> 8);
	writeHex(value);
	
#endif
	return *this;
}

/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>

#include <modm/utils/arithmetic_traits.hpp>
#include <modm/architecture/driver/accessor/flash.hpp>

#include "iostream.hpp"

FLASH_STORAGE(uint16_t base[]) = { 10, 100, 1000, 10000 };

// ----------------------------------------------------------------------------
modm::IOStream::IOStream(IODevice& outputDevice) :
	device(&outputDevice),
	mode(Mode::Ascii)
{
}

// ----------------------------------------------------------------------------
void
modm::IOStream::writeInteger(int16_t value)
{
	if (value < 0) {
		this->device->write('-');
		this->writeInteger(static_cast<uint16_t>(-value));
	}
	else{
		this->writeInteger(static_cast<uint16_t>(value));
	}
}

void
modm::IOStream::writeInteger(uint16_t value)
{
	accessor::Flash<uint16_t> basePtr = modm::accessor::asFlash(base);

	bool zero = true;
	uint8_t i = 4;
	do {
		i--;
		char d;
		for (d = static_cast<uint16_t>('0'); value >= basePtr[i]; value -= basePtr[i])
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
modm::IOStream::writeInteger(int32_t value)
{
#if defined(MODM_CPU_AVR)
	char buffer[ArithmeticTraits<int32_t>::decimalDigits + 1]; // +1 for '\0'

	// Uses the optimized non standard function 'ltoa()' which is
	// not always available.

	this->device->write(ltoa(value, buffer, 10));
#else
	if (value < 0) {
		this->device->write('-');
		this->writeInteger(static_cast<uint32_t>(-value));
	}
	else{
		this->writeInteger(static_cast<uint32_t>(value));
	}
#endif
}

void
modm::IOStream::writeInteger(uint32_t value)
{
#if defined(MODM_CPU_AVR)
	char buffer[ArithmeticTraits<uint32_t>::decimalDigits + 1]; // +1 for '\0'

	// Uses the optimized non standard function 'ultoa()' which is
	// not always available.
	this->device->write(ultoa(value, buffer, 10));
#else
	char buffer[ArithmeticTraits<uint32_t>::decimalDigits + 1]; // +1 for '\0'

	// ptr points to the end of the string, it will be filled backwards
	char *ptr = buffer + ArithmeticTraits<uint32_t>::decimalDigits;

	*ptr = '\0';

	// calculate the string backwards
	do{
		uint32_t quot = value / 10;
		uint8_t rem = value - quot*10;
		*(--ptr) = static_cast<char>(rem) + '0';
		value = quot;
	}while (value != 0);

	// write string
	this->device->write(ptr);
#endif
}

#ifndef MODM_CPU_AVR
void
modm::IOStream::writeInteger(int64_t value)
{
	if (value < 0) {
		this->device->write('-');
		this->writeInteger(static_cast<uint64_t>(-value));
	}
	else{
		this->writeInteger(static_cast<uint64_t>(value));
	}
}

void
modm::IOStream::writeInteger(uint64_t value)
{
	char buffer[ArithmeticTraits<uint64_t>::decimalDigits + 1]; // +1 for '\0'

	// ptr points to the end of the string, it will be filled backwards
	char *ptr = buffer + ArithmeticTraits<uint64_t>::decimalDigits;

	*ptr = '\0';

	// calculate the string backwards
	do{
		uint64_t quot = value / 10;
		uint8_t rem = value - quot*10;
		*(--ptr) = static_cast<char>(rem) + '0';
		value = quot;
	}while (value != 0);

	// write string
	this->device->write(ptr);
}
#endif

// ----------------------------------------------------------------------------
void
modm::IOStream::writeHex(const char* s)
{
	while (*s != '\0') {
		this->writeHex(*s);
		s++;
	}
}

void
modm::IOStream::writeBin(const char* s)
{
	while (*s != '\0') {
		this->writeBin(*s);
		s++;
	}
}

// ----------------------------------------------------------------------------
void
modm::IOStream::writeHexNibble(uint8_t nibble)
{
	char character;
	if (nibble > 9) {
		character = nibble + 'A' - 10;
	}
	else {
		character = nibble + '0';
	}
	this->device->write(character);
}

// ----------------------------------------------------------------------------
void
modm::IOStream::writeHex(uint8_t value)
{
	writeHexNibble(value >> 4);
	writeHexNibble(value & 0xF);
}

void
modm::IOStream::writeBin(uint8_t value)
{
	for (uint_fast8_t ii = 0; ii < 8; ii++)
	{
		if (value & 0x80) {
			this->device->write('1');
		}
		else {
			this->device->write('0');
		}
		value <<= 1;
	}

}

// ----------------------------------------------------------------------------
modm::IOStream&
modm::IOStream::operator << (const myfunc& value)
{
	unsigned char *p = (unsigned char *)&value;

	for (std::size_t i = 0; i < sizeof(myfunc); i++)
	{
		writeHex(p[sizeof(myfunc) - i - 1]);
	}
	return *this;
}

modm::IOStream&
modm::IOStream::operator << (const void* p)
{
#if MODM_SIZEOF_POINTER == 2

	this->device->write('0');
	this->device->write('x');

	uint16_t value = reinterpret_cast<uint16_t>(p);

	writeHex(value >> 8);
	writeHex(value);

#elif MODM_SIZEOF_POINTER == 4

	this->device->write('0');
	this->device->write('x');

	uint32_t value = reinterpret_cast<uint32_t>(p);

	writeHex(value >> 24);
	writeHex(value >> 16);
	writeHex(value >> 8);
	writeHex(value);

#elif MODM_SIZEOF_POINTER == 8

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

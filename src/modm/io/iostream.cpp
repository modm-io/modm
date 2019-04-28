/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2014, 2019 Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "iostream.hpp"
#include <modm/architecture/interface/accessor.hpp>

namespace modm
{

IOStream&
IOStream::get(char* s, size_t n)
{
	if(n < 1) {
		return *this;
	}
	char cc;
	size_t ii;
	for(ii = 0; ii < (n-1); ++ii) {
		if(device->read(cc)) {
			s[ii] = cc;
		} else {
			break;
		}
	}
	s[ii] = '\0';
	return *this;
}

// ----------------------------------------------------------------------------
IOStream&
IOStream::operator << (const bool& v)
{
	switch (mode)
	{
		case Mode::Ascii:
			*this << (v ? IFSS("true") : IFSS("false"));
			break;
		case Mode::Hexadecimal:
			device->write('0');
			// fallthrough
		case Mode::Binary:
			device->write(v ? '1' : '0');
			break;
	}
	return *this;
}

// ----------------------------------------------------------------------------
void
IOStream::writeHex(uint8_t value)
{
	const auto fn_nibble = [this](uint8_t nibble)
	{
		device->write( nibble + (nibble > 9 ? 'A' - 10 : '0') );
	};
	fn_nibble(value >> 4);
	fn_nibble(value & 0xF);
}

// ----------------------------------------------------------------------------
void
IOStream::writeBin(uint8_t value)
{
	for (uint_fast8_t ii = 0; ii < 8; ii++)
	{
		device->write(value & 0x80 ? '1' : '0');
		value <<= 1;
	}
}

// ----------------------------------------------------------------------------
void
IOStream::writePointer(const void* p)
{
	device->write('0');
	device->write('x');
	const uintptr_t value = reinterpret_cast<uintptr_t>(p);

#if MODM_SIZEOF_POINTER == 2

	writeHex(value >> 8);
	writeHex(value);

#elif MODM_SIZEOF_POINTER == 4

	for (uint8_t ii=24; ii < 32; ii -= 8)
		writeHex(value >> ii);

#elif MODM_SIZEOF_POINTER == 8

	for (uint8_t ii=56; ii < 64; ii -= 8)
		writeHex(value >> ii);

#endif
}

IOStream&
black(IOStream& ios)
{ return ios << IFSS("\033[30m"); }

IOStream&
red(IOStream& ios)
{ return ios << IFSS("\033[31m"); }

IOStream&
green(IOStream& ios)
{ return ios << IFSS("\033[32m"); }

IOStream&
yellow(IOStream& ios)
{ return ios << IFSS("\033[33m"); }

IOStream&
blue(IOStream& ios)
{ return ios << IFSS("\033[34m"); }

IOStream&
magenta(IOStream& ios)
{ return ios << IFSS("\033[35m"); }

IOStream&
cyan(IOStream& ios)
{ return ios << IFSS("\033[36m"); }

IOStream&
white(IOStream& ios)
{ return ios << IFSS("\033[37m"); }

} // namespace modm
